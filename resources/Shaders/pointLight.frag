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
    vec3 dirToLight = normalize(u_LightPosition - worldPosition);
    vec3 dirFromLight = -dirToLight;
    vec3 cameraDir = -normalize(u_CameraPosition - worldPosition);
    vec3 normal = normalize(worldNormal);

	// Ambient Light
    vec3 ambient = u_K.x * u_LightColor;

    // Diffuse Light
    float diffuseFactor = max(0.0, dot(normal, dirToLight));
    vec3 diffuse = u_K.y * u_LightColor * diffuseFactor;

    // Specular Light
    vec3 reflectedLight = normalize(reflect(dirFromLight, normal));
    float specularFactor = max(0.0, dot(cameraDir, reflectedLight));
    specularFactor = pow(specularFactor, u_K.w);
    vec3 specular = u_K.z * specularFactor * u_LightColor;

    vec3 result = (ambient + diffuse + specular);

    gl_FragColor = vec4(result, 1.0) * u_Color * texture2D(u_Texture, uv);
}
