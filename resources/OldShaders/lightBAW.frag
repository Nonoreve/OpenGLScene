#version 140
precision mediump float;

varying vec3 Normal;
varying vec3 LightPos;
varying vec3 FragPos;

uniform vec3 u_LightPosition; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_ModelView;

uniform vec3 u_LightColor;

void main()
{

	vec4 u_Color = vec4(1.0,1.0,1.0,1.0);

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_LightColor;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_LightColor; 
    
    vec3 result = (ambient + diffuse + specular) * u_Color.xyz;
    gl_FragColor = vec4(result, 1.0);
}
