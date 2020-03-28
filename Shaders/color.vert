#version 330
precision mediump float;

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * vPosition;
	v_TexCoord = vTexCoord;
}
