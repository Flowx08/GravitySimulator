#include "simulator.hpp"
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <assert.h>
#include <type_traits>
#include "Renderer.hpp"
#include "SDL2/SDL_keycode.h"
#include "config.hpp"
#include <fstream>
#include <sstream>

Simulator::Simulator()
{
	movementSpeed = 1.0;
	playlenght = 0;
	memoryUsage = 0;
	mode = 0;
	t = 0;
	paused = false;
	
	//settings
	gForce = config::gForce;
	record = config::record;
	viewZoom = config::startViewZoom;
	recordLimit = config::recordLimit;
	recordForT = config::recordForT;
	playFromRecord = config::playFromRecord;
	recordFilename = "recording.bin";
	showUI = config::showUI;
	drawParticles = config::drawParticles;
}

void Simulator::quit()
{
	onEnd();

	//save positions recording
	if (record && !playFromRecord)
	{
		printf("Saving recording...\n");
		std::ofstream filepos("recording.bin", std::ios::out | std::ofstream::binary);
		filepos.write(reinterpret_cast<const char *>(&t), sizeof(int));
		filepos.write(reinterpret_cast<const char *>(&startPositions[0]), sizeof(int32_t) * startPositions.size());
		filepos.write(reinterpret_cast<const char *>(&predictonErrors[0]), sizeof(int8_t) * predictonErrors.size());
		printf("Saved recording!\n");
	}
}
	
void Simulator::init(Renderer& r, unsigned int particlesCount)
{
	initializeParticles(r, particlesCount);
	
	//Load recorded simulation results
	if (playFromRecord) {
		std::ifstream filep(recordFilename, std::ios::in | std::ifstream::binary);
		filep.read(reinterpret_cast<char *>(&playlenght), sizeof(unsigned int));
		printf("playlengh: %d\n", playlenght);

		startPositions = std::vector<int32_t>(particlesCount * 2 * 3);
		filep.read(reinterpret_cast<char *>(&startPositions[0]), sizeof(int32_t) * startPositions.size());
		
		predictonErrors = std::vector<int8_t>(particlesCount * 2 * (playlenght - 3));
		filep.read(reinterpret_cast<char *>(&predictonErrors[0]), sizeof(int8_t) * predictonErrors.size());

		printf("recording loaded!\n");
	}
	
	
	//store buffers fore encoding and decoding compressed data
	pos = std::vector<int32_t>(particlesCount * 2);
	prediction = std::vector<int32_t>(particlesCount * 2);
}

bool Simulator::step(Renderer& r)
{
	//Handling stopping recording 
	if (playFromRecord && t == playlenght) {
		return false;
	}
	if (record && recordLimit) {
		if (t >= recordForT) return false;	
	}

	//Handle movement
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_D]) {
		viewX += movementSpeed;	
		movementSpeed += 1.0;
	}
	else if (keys[SDL_SCANCODE_A]) {
		viewX -= movementSpeed;
		movementSpeed += 1.0;
	}
	else if (keys[SDL_SCANCODE_W]) {
		viewY -= movementSpeed;	
		movementSpeed += 1.0;
	}
	else if (keys[SDL_SCANCODE_S]) {
		viewY += movementSpeed;	
		movementSpeed += 1.0;
	}
	else
	{
		movementSpeed = 1.0;
	}

	//Handle zoom in and out
	if (keys[SDL_SCANCODE_E])
		viewZoom *= 1.01;	
	if (keys[SDL_SCANCODE_R])
		viewZoom *= 0.99;	

	
	if (playFromRecord)
	{

		int32_t fx, fy;
		if (t < 3)
		{
			for (int i = 0; i < particlesCount; i++)
			{
				fx = (int)startPositions[t * particlesCount * 2 + i * 2 + 0];
				fy = (int)startPositions[t * particlesCount * 2 + i * 2 + 1];
				r.drawPoint((fx-viewX)*viewZoom -viewHW, (fy - viewY) * viewZoom - viewHH,
						1.0, 1.0, 1.0, (particles[i].mass / 2.1) * 0.6);
			}
		}
		else
		{
			if (!paused) {
				//predict next position and store old coordinates
				int32_t px, py;
				for (int i = 0; i < particlesCount; i++) {

					//compute prediction based on statistics
					px = pos[i * 2 + 0] + posDiffs[i * 2 + 0] + velDiffs[i * 2 + 0];
					py = pos[i * 2 + 1] + posDiffs[i * 2 + 1] + velDiffs[i * 2 + 1];
					prediction[i * 2 + 0] = px;
					prediction[i * 2 + 1] = py;
				}

				//decode
				for (int i = 0; i < particlesCount; i++)
				{
					//compute next point position based on prediction + prediction error
					fx = prediction[i * 2 + 0] + predictonErrors[(i + particlesCount * (t - 3)) * 2 + 0];
					fy = prediction[i * 2 + 1] + predictonErrors[(i + particlesCount * (t - 3)) * 2 + 1];

					//update statistics
					int32_t vx = (int32_t)fx - pos[i * 2 + 0];
					int32_t vy = (int32_t)fy - pos[i * 2 + 1];
					velDiffs[i * 2 + 0] = vx - posDiffs[i * 2 + 0];
					velDiffs[i * 2 + 1] = vy - posDiffs[i * 2 + 1];
					posDiffs[i * 2 + 0] = vx;
					posDiffs[i * 2 + 1] = vy;
					pos[i * 2 + 0] = fx;
					pos[i * 2 + 1] = fy;

				}
			}
				
			//draw points
			for (int i = 0; i < particlesCount; i++)
			{
				fx = pos[i * 2 + 0];
				fy = pos[i * 2 + 1];
				r.drawPoint(((float)fx-viewX)*viewZoom -viewHW, ((float)fy - viewY) * viewZoom - viewHH,
						1.0, 1.0, 1.0, (particles[i].mass / 2.1) * 0.6);
			}
		}

		//compute statistics 	
		if (t == 2) {

			//compute velocities
			for (int tk = 1; tk < 3; tk++) {
				for (int i = 0; i < particlesCount; i++) {
					float x1 = startPositions[(i + particlesCount * (tk - 1)) * 2 + 0];
					float x2 = startPositions[(i + particlesCount * tk) * 2 + 0];
					float dx = x2 - x1;

					float y1 = startPositions[(i + particlesCount * (tk - 1)) * 2 + 1];
					float y2 = startPositions[(i + particlesCount * tk) * 2 + 1];
					float dy = y2 - y1;

					posDiffs.push_back(dx);
					posDiffs.push_back(dy);
				}
			}
			
			//compute accelerations
			for (int tk = 1; tk < 2; tk++) {
				for (int i = 0; i < particlesCount; i++) {
					float vx1 = posDiffs[(i + particlesCount * (tk -1)) * 2 + 0];
					float vx2 = posDiffs[(i + particlesCount * tk) * 2 + 0];
					float dvx = vx2 - vx1;

					float vy1 = posDiffs[(i + particlesCount * (tk - 1)) * 2 + 1];
					float vy2 = posDiffs[(i + particlesCount * tk) * 2 + 1];
					float dvy = vy2 - vy1;

					velDiffs.push_back(dvx);
					velDiffs.push_back(dvy);
				}
			}

			//copy posDiff in first position
			for (int i = 0; i < particlesCount; i++) {
				posDiffs[i * 2 + 0] = posDiffs[(i + particlesCount) * 2 + 0];
				posDiffs[i * 2 + 1] = posDiffs[(i + particlesCount) * 2 + 1];
			}

			//store latest positions
			for (int i = 0; i < particlesCount; i++) {
				pos[i * 2 + 0] = startPositions[(i + particlesCount * 2) * 2 + 0];
				pos[i * 2 + 1] = startPositions[(i + particlesCount * 2) * 2 + 1];
			}

		}

	}
	else
	{
		if (record && t >= 3) 
		{
			//predict next position and store old coordinates
			int32_t px, py;
			for (int i = 0; i < particlesCount; i++) {
				//compute prediction
				px = pos[i * 2 + 0] + posDiffs[i * 2 + 0] + velDiffs[i * 2 + 0];
				py = pos[i * 2 + 1] + posDiffs[i * 2 + 1] + velDiffs[i * 2 + 1];
				prediction[i * 2 + 0] = px;
				prediction[i * 2 + 1] = py;
			}

		}

		update(r);	

		for (int i = 0; i < particlesCount; i++) {
			if (record) {
				if (t < 3)
				{
					//record exact position with full precision
					startPositions.push_back((int32_t)particles[i].x);
					startPositions.push_back((int32_t)particles[i].y);
				}
				else
				{
					//update statistics
					int32_t vx = (int32_t)particles[i].x - pos[i * 2 + 0];
					int32_t vy = (int32_t)particles[i].y - pos[i * 2 + 1];
					velDiffs[i * 2 + 0] = vx - posDiffs[i * 2 + 0];
					velDiffs[i * 2 + 1] = vy - posDiffs[i * 2 + 1];
					posDiffs[i * 2 + 0] = vx;
					posDiffs[i * 2 + 1] = vy;
					pos[i * 2 + 0] = (int32_t)particles[i].x;
					pos[i * 2 + 1] = (int32_t)particles[i].y;

					//compute and store prediction error
					int32_t errX = (int32_t)particles[i].x - prediction[i * 2 + 0];
					int32_t errY = (int32_t)particles[i].y - prediction[i * 2 + 1];
					assert(errX > -100 && errX < 100);
					assert(errY > -100 && errY < 100);
					predictonErrors.push_back((int8_t)errX);
					predictonErrors.push_back((int8_t)errY);
				}
			}
		}
	
		//compute statistics
		if (t == 2) {

			//compute velocities
			for (int tk = 1; tk < 3; tk++) {
				for (int i = 0; i < particlesCount; i++) {
					float x1 = startPositions[(i + particlesCount * (tk - 1)) * 2 + 0];
					float x2 = startPositions[(i + particlesCount * tk) * 2 + 0];
					float dx = x2 - x1;

					float y1 = startPositions[(i + particlesCount * (tk - 1)) * 2 + 1];
					float y2 = startPositions[(i + particlesCount * tk) * 2 + 1];
					float dy = y2 - y1;

					posDiffs.push_back(dx);
					posDiffs.push_back(dy);
				}
			}

			//compute accelerations
			for (int tk = 1; tk < 2; tk++) {
				for (int i = 0; i < particlesCount; i++) {
					float vx1 = posDiffs[(i + particlesCount * (tk -1)) * 2 + 0];
					float vx2 = posDiffs[(i + particlesCount * tk) * 2 + 0];
					float dvx = vx2 - vx1;

					float vy1 = posDiffs[(i + particlesCount * (tk - 1)) * 2 + 1];
					float vy2 = posDiffs[(i + particlesCount * tk) * 2 + 1];
					float dvy = vy2 - vy1;

					velDiffs.push_back(dvx);
					velDiffs.push_back(dvy);
				}
			}

			//store latest positions
			for (int i = 0; i < particlesCount; i++) {
				pos[i * 2 + 0] = startPositions[(i + particlesCount * 2) * 2 + 0];
				pos[i * 2 + 1] = startPositions[(i + particlesCount * 2) * 2 + 1];
			}

			//copy posDiff in first position
			for (int i = 0; i < particlesCount; i++) {
				posDiffs[i * 2 + 0] = posDiffs[(i + particlesCount) * 2 + 0];
				posDiffs[i * 2 + 1] = posDiffs[(i + particlesCount) * 2 + 1];
			}
		}
	}

	if (!paused)
		t++;

	return true;
}

void Simulator::initializeParticles(Renderer& r, unsigned int count) {}
bool Simulator::update(Renderer& r) {return false;}
void Simulator::onEnd() {};
void Simulator::costumHandleKeyDown(unsigned int key) {}


void Simulator::handleKeyDown(unsigned int key) 
{
	if (key == SDLK_i) {
		mode = 1;
	}
	else if (key == SDLK_ESCAPE) {
		mode = 0;
		inputText = "";
	}
	else if (key == SDLK_p) {
		paused = !paused;
	}

	costumHandleKeyDown(key);
}

void Simulator::handleTextInput(char c)
{
	if (mode == 1) {
		inputText += c;
	}
}

void Simulator::drawUI(Renderer& r)
{
	if (!showUI) return;

	//Get screen size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	Uint32 windowWidth = dm.w;
	Uint32 windowHeight = dm.h;
	
	//Draw log
	r.drawText("fps:\nt:\ncpu_cores:\nused_mem:\nparticles:\nview_cx:\nview_cy:\nview_zoom:\ngforce:\nmin_f_dist:\ndraw:", 8, 8, 1.2, 1, 1, 1, 1);
	
	char buff[512];
	snprintf(buff, sizeof(buff), "%.4f\n%d\n%d\n%.2f MB\n%d\n%.4f\n%.4f\n%.4f\n%.4f\n%.4f\n%s",
		FPS, t, config::cpuCores, memoryUsage, particlesCount, viewX, viewY, viewZoom, gForce, config::minForceDistance, drawParticles ? "true" : "false");
	r.drawText(buff, 128 + 8, 8, 1.2, 1, 1, 1, 1);
	
	//Draw input text
	if (mode == 1) {
		r.drawText((":" + inputText).c_str(), 8,  windowHeight - 14 - 8, 1.2, 1, 1, 1, 1);
	}
}
