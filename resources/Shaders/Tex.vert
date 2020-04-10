#version 330
precision mediump float;

layout (location = 0) in vec4 v_Position;
layout (location = 1) in vec2 v_UV;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

out vec2 uv;

void main() {
    uv = v_UV;
	// Hence x and y go from -w to w hence -1 to +1. Premultiply this variable if you want to transform the position.
	gl_Position = u_Projection * u_ModelView * v_Position;
}

