#version 330
precision mediump float;

in vec2 uv;

uniform sampler2D u_Texture;

layout (location = 0) out vec4 color;

void main() {
    vec4 texColor = texture2D(u_Texture, uv);
    color = vec4(1.0, 0.0, 0.0);//texColor;
}
