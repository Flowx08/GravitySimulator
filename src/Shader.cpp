#include "Shader.hpp"
#include <fstream>
#include <iostream>
#include <vector>

#define GL_SILENCE_DEPRECATION
#if defined(__linux__)
#include <GL/glew.h>
#include <GL/gl.h>
#else
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#endif

std::string readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cout << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

Shader::Shader()
{
	program = -1;
}

Shader::~Shader() 
{
	if (program != -1) {
		glDeleteProgram(program);
	}
}

Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	loadFromFile(vertexShaderPath, fragmentShaderPath);
}

void Shader::loadFromFile(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shaders

    std::string vertShaderStr = readFile(vertexShaderPath.c_str());
    std::string fragShaderStr = readFile(fragmentShaderPath.c_str());
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader

    std::cout << "Compiling vertex shader." << std::endl;
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    // Check vertex shader

    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
	if (vertShaderError.size() != 1)
		std::cout << &vertShaderError[0] << std::endl;

    // Compile fragment shader

    std::cout << "Compiling fragment shader." << std::endl;
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    // Check fragment shader

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
	if (fragShaderError.size() != 1)
		std::cout << &fragShaderError[0] << std::endl;

    std::cout << "Linking program" << std::endl;
    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError( (logLength > 1) ? logLength : 1 );
    glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
	if (programError.size() != 1)
		std::cout << &programError[0] << std::endl;

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

int Shader::getProgram()
{
	return program;
}

void Shader::apply()
{
	glUseProgram(program);
}

