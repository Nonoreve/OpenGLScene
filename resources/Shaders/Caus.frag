#version 140
precision mediump float;

varying vec3 Normal;
varying vec3 LightPos;
varying vec3 FragPos;
varying vec2 uv;

uniform sampler2D u_Texture;
uniform sampler2D u_Caustic;

uniform vec3 u_LightPosition; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_ModelView;

uniform vec3 u_LightColor;
uniform vec4 u_Color;

uniform vec4 u_K;


vec4 fogColor;

varying float visibility;

void main()
{
    // ambient
    //float ambientStrength = 0.1;
    float ambientStrength = u_K.x;
	vec3 ambient = ambientStrength * u_LightColor;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //float diffStrength = 1.0
	float diffStrength = u_K.y;
	
	vec3 diffuse = diff * u_LightColor *diffStrength;
    
    // specular
    //float specularStrength = 0.5;
    float specularStrength = u_K.z;
	
	vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_K.w);//anciennement 32
    vec3 specular = specularStrength * spec * u_LightColor; 
    
    vec4 result = vec4((ambient + diffuse + specular) * u_Color.xyz,u_Color.w);
	vec4 tc = texture2D(u_Texture, uv);
	vec4 cc = texture2D(u_Caustic, uv * 10);
	
	vec4 finalTex = result * tc * cc;
	
	fogColor = vec4(0.2, 0.2, 0.6, 1.0);
	
	gl_FragColor = mix(fogColor, finalTex, visibility);
	
}
