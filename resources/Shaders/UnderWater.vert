#version 140
precision mediump float;

attribute vec4 v_Position;
attribute vec2 v_UV;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

varying vec2 uv;

varying float visibility;
//0.02 est ok avec gradient 1.5 (legerement baisser gradient)
const float density = 0.2;//0.007
const float gradient = 1.5;//1.5

void main() {
    uv = v_UV;
    // Hence x and y go from -w to w hence -1 to +1. Premultiply this variable if you want to transform the position.
	vec4 finalPos = u_Projection * u_ModelView * v_Position;
	
	gl_Position = finalPos;
	
	float distance = length(finalPos.xyz);
    visibility = exp(-pow((distance*density),gradient));//Calcul plus réaliste mais moin beau
   
    //visibility = exp(-distance * density);
    visibility = clamp (visibility, 0.0, 1.0);
	
}

