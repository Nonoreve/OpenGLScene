#version 140
precision mediump float;

attribute vec4 v_Position;
attribute vec2 v_UV;
attribute vec3 v_Normal;

uniform mat4 u_Projection;
uniform mat4 u_ModelView;

varying vec3 worldPosition;
varying vec3 worldNormal;
varying vec2 uv;

void main() {
    gl_Position = u_Projection * u_ModelView * v_Position;

    vec4 _worldPos = u_ModelView * v_Position;
    worldPosition = _worldPos.xyz / _worldPos.w;

    worldNormal = normalize(transpose(mat3(u_ModelView)) * v_Normal);
    uv = v_UV;
}
