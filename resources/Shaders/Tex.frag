#version 140
precision mediump float;

varying vec2 uv;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main() {
    vec4 texColor = texture2D(u_Texture, uv);
    gl_FragColor = u_Color * texColor;
}
