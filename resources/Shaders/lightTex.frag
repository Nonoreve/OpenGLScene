#version 330
precision mediump float;

in vec3 worldNormal;
in vec3 worldPosition;
in vec2 uv;

uniform sampler2D u_Texture;
uniform vec4 u_K;
uniform vec3 u_Color;
uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPosition;

layout (location = 0) out vec4 color;

void main() {
    vec3 L = normalize(u_LightPosition - worldPosition);
    vec3 V = normalize(u_CameraPosition - worldPosition);
    vec3 N = normalize(worldNormal);

    vec3 ambient = u_K.x * u_Color * u_LightColor;
    vec3 diffuse = u_K.y * u_Color * u_LightColor * max(0.0, dot(N,L)); // on prend N ou varyWolrNormal ?
    vec3 specular = u_K.z * pow(max(0.0,dot(reflect(-L, N), -V)), u_K.w) * u_LightColor;
    
    vec3 result = (ambient + diffuse + specular); // voir si le specular prend en compte la couleur

    color = vec4(result,1.0) * texture2D(u_Texture, uv).abgr;
}
