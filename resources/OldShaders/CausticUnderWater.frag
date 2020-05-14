#version 140
precision mediump float;

varying vec2 uv;

uniform sampler2D u_Texture;
uniform sampler2D u_Caustic;

uniform vec4 u_Color;

vec4 fogColor;

varying float visibility;

void main() {
    vec4 texColor = texture2D(u_Texture, uv);
    vec4 causColor = texture2D(u_Caustic, uv * 10);
	
	vec4 finalColor = u_Color * texColor * causColor;
	
	fogColor = vec4(0.2, 0.2, 0.6, 1.0);
	
    gl_FragColor = mix(fogColor, finalColor, visibility);
    //gl_FragColor = finalColor;
}
