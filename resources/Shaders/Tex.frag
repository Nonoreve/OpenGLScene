#version 140
precision mediump float;

varying vec2 uv;

uniform sampler2D u_Texture;

void main() {
    vec4 texColor = texture2D(u_Texture, uv);
    gl_FragColor = texColor;
    //gl_FragColor = vec4(0.0, 0.0, 1.0);;
}
