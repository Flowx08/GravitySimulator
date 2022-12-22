#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

class Shader
{
public:

	Shader();
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
	void loadFromFile(std::string vertexShaderPath, std::string fragmentShaderPath);
	~Shader();
	
	void apply();
	int getProgram();

private:
	int program;
};

#endif // !SHADER_HPP
