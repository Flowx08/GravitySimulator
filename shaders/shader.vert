#version 150 core

uniform mat4 projmat;

in vec2 position;
in vec4 color;
out vec4 vertColor;
void main()
{
    gl_Position = projmat * vec4(position, 1.0, 1.0);
    vertColor = color;
}
