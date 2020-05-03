#version 140
precision mediump float;

varying vec3 worldNormal;
varying vec3 worldPosition;
varying vec2 uv;

uniform sampler2D u_Texture;
uniform vec4 u_Color;
uniform vec4 u_K;
uniform vec3 u_LightColor;
uniform vec3 u_LightPosition;
uniform vec3 u_CameraPosition;

void main() {
    vec3 L = normalize(u_LightPosition - worldPosition);
    vec3 V = normalize(u_CameraPosition - worldPosition);
    vec3 N = normalize(worldNormal);

    vec3 ambient = u_K.x * u_Color.xyz * u_LightColor;
    vec3 diffuse = u_K.y * u_Color.xyz * u_LightColor * max(0.0, dot(N,L));
    vec3 specular = u_K.z * pow(max(0.0, dot(reflect(-L, N), -V)), u_K.w) * u_LightColor;
    
    vec4 result = vec4((ambient + diffuse + specular), u_Color.w);

    gl_FragColor = result * texture2D(u_Texture, uv);
}
