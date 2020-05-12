#version 140
precision mediump float;

attribute vec4 v_Position;
attribute vec3 v_Normal;
attribute vec2 v_UV;

varying vec3 Normal;
varying vec3 LightPos;
varying vec3 FragPos;

varying vec2 uv;

uniform vec3 u_LightPosition;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_ModelView;


varying float visibility;
//0.02 est ok avec gradient 1.5 (legerement baisser gradient)
const float density = 0.10;//0.007
const float gradient = 1.0;//1.5

void main()
{
	
	uv = v_UV;
	
	vec4 finalPos = u_Projection * u_ModelView * v_Position;
	
    gl_Position = finalPos;
	
    FragPos = vec3(u_ModelView * v_Position);
    Normal = mat3(transpose(inverse(u_ModelView))) * v_Normal;
    LightPos = vec3(u_View * vec4(u_LightPosition, 1.0)); // Transform world-space light position to view-space light position

	float distance = length(finalPos.xyz);
    visibility = exp(-pow((distance*density),gradient));//Calcul plus réaliste mais moin beau
   
    //visibility = exp(-distance * density);
    visibility = clamp (visibility, 0.0, 1.0);
	
}
