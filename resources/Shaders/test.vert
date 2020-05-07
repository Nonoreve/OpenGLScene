#version 140
precision mediump float;

attribute vec4 v_Position;
attribute vec2 v_UV;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

uniform mat4 u_Model;
uniform mat4 u_View;

varying vec2 uv;

void main() {
    uv = v_UV;
	
	vec4 truc = u_Model * v_Position;
	
	gl_Position = u_Projection * u_ModelView * v_Position;
}

