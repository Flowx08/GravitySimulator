#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <vector>
#include "Shader.hpp"

class Renderer
{
public:
	Renderer();
	~Renderer();
	void init(SDL_Window* window);
	void drawPoint(float x, float y, float r, float g, float b, float a);
	void drawText(const char* text, float x, float y, float scale, float r, float g, float b, float a);
	void clear();
	void present();

	SDL_Renderer* getSDLRenderer();

private:
	SDL_Renderer* r;
	SDL_Window* window;
	
	SDL_GLContext glContext;
	Shader shaderProgram;
	unsigned int vao;
	unsigned int vbo;
	unsigned int cbo;
	std::vector<float> verticies;
	std::vector<float> colors;

	
};

#endif // !RENDERER_HPP
