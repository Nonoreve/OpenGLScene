#version 140
precision mediump float;

varying vec2 uv;

uniform sampler2D u_Texture;
uniform sampler2D caus_Texture;

uniform vec4 u_Color;

vec3 fogColor;//pas utilise


varying float visibility;

void main() {
    vec4 texColor = texture2D(u_Texture, uv);
	
	vec4 finalColor = u_Color * texColor;
	
	fogColor = vec3(0.2,0.2,0.6);
	
    gl_FragColor = mix(vec4(fogColor,1.0),finalColor,visibility);
}
