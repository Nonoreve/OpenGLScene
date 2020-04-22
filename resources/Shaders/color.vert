#version 330
precision mediump float;

layout (location = 0) in vec4 v_Position;
layout (location = 1) in vec2 v_TexCoord;

uniform mat4 u_MVP;

out vec2 texCoord;

void main() {
	gl_Position = u_MVP * v_Position;
	texCoord = v_TexCoord;
}