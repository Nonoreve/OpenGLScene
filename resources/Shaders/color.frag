#version 330
precision mediump float;

in vec2 texCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

layout (location = 0) out vec4 color;

void main() {
	vec4 texColor = texture(u_Texture, texCoord);
    color = texColor;
}