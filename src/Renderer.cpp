#include "Renderer.hpp"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include <iostream>

#if defined(__linux__)
 //#include <GLES3/gl3.h>
 //#include <GL/glu.h>
 #include <GL/glew.h>
 #else
 #include <OpenGL/gl3.h>
 #include <OpenGL/glu.h>
 #endif

#define GLT_IMPLEMENTATION
#include "gltext.h"

// set the OpenGL orthographic projection matrix
void makeOrthoProj( 
    const float b, const float t, const float l, const float r, 
    const float n, const float f, 
    float* M) 
{ 
    // set OpenGL perspective projection matrix
    M[0] = 2 / (r - l); 
    M[4] = 0; 
    M[8] = 0; 
    M[12] = 0; 
 
    M[1] = 0; 
    M[5] = 2 / (t - b); 
    M[9] = 0; 
    M[13] = 0; 
 
    M[2] = 0; 
    M[6] = 0; 
    M[11] = -2 / (f - n); 
    M[14] = 0; 
 
    M[3] = -(r + l) / (r - l); 
    M[7] = -(t + b) / (t - b); 
    M[12] = -(f + n) / (f - n); 
    M[15] = 1; 
}

void checkGLError()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR){
        std::cout << err;
    }  
}

Renderer::Renderer()
{
	r = NULL;
	glContext = NULL;
}

Renderer::~Renderer()
{
} 

void Renderer::init(SDL_Window* window) {
	this->window = window;

	//Get screen size
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	Uint32 windowWidth = dm.w;
	Uint32 windowHeight = dm.h;

	//Set OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	//Initilize Opengl
	glContext = SDL_GL_CreateContext(window);
	if (glContext == NULL) {
		printf("Couldn't create OpenGL context\n");
	}
	SDL_GL_SetSwapInterval(0);

#if defined(__linux__)
	GLenum err = glewInit();
	if (err != GLEW_OK)
		exit(1); 
#endif

	const float ratio = (float) windowWidth / (float) windowHeight;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	//glEnable(GL_DEPTH_TEST); // enable depth-testing
    //glDepthFunc(GL_LESS);

	glClearColor( 0, 0, 0, 0 );

	//glMatrixMode( GL_PROJECTION );
	//glLoadIdentity( );
	//glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

	//glMatrixMode(GL_MODELVIEW);
	
	//Initilize glText
	if (!gltInit())
	{
		fprintf(stderr, "Failed to initialize glText\n");
		return;
	}
	gltViewport(windowWidth, windowHeight);

	//Initlize shader
	shaderProgram.loadFromFile("./shaders/shader.vert", "./shaders/shader.frag");
	shaderProgram.apply();

	//bind locations
	const int positionLoc = glGetAttribLocation(shaderProgram.getProgram(), "position");
	const int colorLoc = glGetAttribLocation(shaderProgram.getProgram(), "color");

	//create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Create vertex buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticies.size(), &verticies[0], GL_STATIC_DRAW);
	glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionLoc);
	
	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), &colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorLoc);

	//projection
	glViewport( 0, 0, windowWidth, windowHeight );
	float matrix[16];
	makeOrthoProj(windowHeight, 0, 0, windowWidth, -1, 1, matrix);
	GLuint projmatLoc = glGetUniformLocation(shaderProgram.getProgram(), "projmat");
	glUniformMatrix4fv(projmatLoc, 1, GL_TRUE, matrix);

	checkGLError();

}

void Renderer::drawPoint(float x, float y, float r, float g, float b, float a)
{
	verticies.push_back(x);
	verticies.push_back(y);
	colors.push_back(r);
	colors.push_back(g);
	colors.push_back(b);
	colors.push_back(a);
}

void Renderer::drawText(const char* text, float x, float y, float scale, float r, float g, float b, float a)
{
	// Creating text
	GLTtext *textBuf = gltCreateText();
	gltSetText(textBuf, text);
	
	//draw the text
	gltBeginDraw();
	gltColor(r, g, b, a);
	gltDrawText2D(textBuf, x, y, scale);
	gltEndDraw();
}

void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
	verticies.clear();
	colors.clear();
}

void Renderer::present()
{
	//Create vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticies.size(), &verticies[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), &colors[0], GL_STATIC_DRAW);
	
	shaderProgram.apply();
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, verticies.size() / 2);
	glFlush();
	SDL_GL_SwapWindow(window);
}

SDL_Renderer* Renderer::getSDLRenderer() {
	return r;
}
