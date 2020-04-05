#version 330
precision mediump float;

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 v_Normal;

uniform mat4 u_MVP;
uniform mat4 u_ModelView;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;

void main() {
    gl_Position = u_MVP * vec4(v_Position , 1.0);

    vec4 _worldPos = u_ModelView * vec4(v_Position, 1.0);
    worldPosition = _worldPos.xyz / _worldPos.w;

    worldNormal = normalize(transpose(mat3(u_ModelView)) * v_Normal);
    uv = v_UV;
}
