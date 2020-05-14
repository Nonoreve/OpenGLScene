#version 140
precision mediump float;

attribute vec4 v_Position;
attribute vec2 v_UV;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

varying vec2 uv;

void main() {
    uv = v_UV;
    // Hence x and y go from -w to w hence -1 to +1. Premultiply this variable if you want to transform the position.
	gl_Position = u_Projection * u_ModelView * v_Position;
}

