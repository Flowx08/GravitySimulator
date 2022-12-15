#include "simulator.hpp"
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <assert.h>
#include <type_traits>
#include "config.hpp"

Simulator::Simulator()
{
	movementSpeed = 1.0;
	playlenght = 0;
	memoryUsage = 0;
	font = NULL;
	
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

void Simulator::initializeParticles(SDL_Renderer* r, unsigned int count) {}
bool Simulator::update(SDL_Renderer* r, unsigned int t) {return false;}
void Simulator::onEnd(unsigned int t) {};
void Simulator::handleKeyDown(unsigned int key) {}

void Simulator::drawUI(SDL_Renderer* r, unsigned int t)
{
	if (!showUI) return;

	if (font == NULL) {
		font = FC_CreateFont();
		FC_LoadFont(font, r, "menlo-regular.ttf", 14, FC_MakeColor(255,255,255,255), TTF_STYLE_NORMAL);  
	}

	SDL_Rect rect;
	rect.x = 8;
	rect.y = 8;
	rect.w = 200;
	rect.h = 94;

	SDL_SetRenderDrawColor(r, 0, 0, 0, 150);
	SDL_RenderFillRect(r, &rect);

	FC_Draw(font, r,  8,  8, "fps:\nt:\ncpu_cores:\nused_mem:\nparticles:\nview_cx:\nview_cy:\nview_zoom:\ngforce:\nmin_f_dist:\ndraw:");
	FC_Draw(font, r,  8 + 100,  8, "%.4f\n%d\n%d\n%.2f MB\n%d\n%.4f\n%.4f\n%.4f\n%.4f\n%.4f\n%s",
			FPS, t, config::cpuCores, memoryUsage, particlesCount, viewX, viewY, viewZoom, gForce, config::minForceDistance, drawParticles ? "true" : "false");
}
