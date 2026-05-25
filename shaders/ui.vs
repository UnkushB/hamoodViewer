#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 texture_coords;

uniform mat4 orthProj;

void main()
{
	texture_coords = uv;

	gl_Position = orthProj *  vec4(position, 1.0);
}