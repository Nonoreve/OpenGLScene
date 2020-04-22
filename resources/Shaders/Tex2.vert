#version 140
precision mediump float;

attribute vec3 v_Position; //Depending who compiles, these variables are not "attribute" but "in". In this version (130) both are accepted. in should be used later

attribute vec2 v_UV;

uniform mat4 u_ModelView;
uniform mat4 u_Projection;

varying vec2 vary_uv; //Depending who compiles, these variables are not "varying" but "out". In this version (130) both are accepted. out should be used later


//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main()
{
    vary_uv = v_UV;
	gl_Position = u_Projection * u_ModelView * vec4(v_Position , 1.0); //We need to put vPosition as a vec4. Because vPosition is a vec3, we need one more value (w) which is here 1.0. Hence x and y go from -w to w hence -1 to +1. Premultiply this variable if you want to transform the position.
}

