#version 330
precision mediump float;

layout (location = 0) in vec4 v_Position;
layout (location = 1) in vec2 v_UV;
layout (location = 2) in vec3 v_Normal;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;

void main() {
    gl_Position = u_Projection * u_ModelView * v_Position;

    vec4 _worldPos = u_ModelView * v_Position;
    worldPosition = _worldPos.xyz / _worldPos.w;

    worldNormal = normalize(transpose(mat3(u_ModelView)) * v_Normal);
    uv = v_UV;
}
