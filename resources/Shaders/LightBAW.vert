#version 140
precision mediump float;

attribute vec4 v_Position;
attribute vec3 v_Normal;

varying vec3 Normal;
varying vec3 LightPos;
varying vec3 FragPos;

uniform vec3 u_LightPosition; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_ModelView;

void main()
{
    gl_Position = u_Projection * u_ModelView * v_Position;
    FragPos = vec3(u_ModelView * v_Position);
    Normal = mat3(transpose(inverse(u_ModelView))) * v_Normal;
    LightPos = vec3(u_View * vec4(u_LightPosition, 1.0)); // Transform world-space light position to view-space light position
}
