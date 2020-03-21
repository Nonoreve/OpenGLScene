#version 130
precision mediump float;

attribute vec3 vPosition; //Depending who compiles, these variables are not "attribute" but "in". In this version (130) both are accepted. in should be used later
attribute vec3 vColor;

varying vec4 varyColor; //Depending who compiles, these variables are not "varying" but "out". In this version (130) both are accepted. out should be used later

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main()
{
	gl_Position = vec4(vPosition, 1.0); //We need to put vPosition as a vec4. Because vPosition is a vec3, we need one more value (w) which is here 1.0. Hence x and y go from -w to w hence -1 to +1. Premultiply this variable if you want to transform the position.
    varyColor = vec4(vColor, 1.0);
}
