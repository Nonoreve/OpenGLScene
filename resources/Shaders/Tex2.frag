#version 140
precision mediump float; //Medium precision for float. highp and smallp can also be used

varying vec2 vary_uv;

uniform sampler2D u_Texture;

//Sometimes we use "out" instead of "varying". "out" should be used in later version of GLSL.

//We still use varying because OpenGLES 2.0 (OpenGL Embedded System, for example for smartphones) does not accept "in" and "out"

void main()
{
    vec4 color = texture2D(u_Texture,vary_uv);

    
    gl_FragColor = color.abgr;
}
