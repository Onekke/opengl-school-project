#version 140

struct Material {
	vec3  ambient;       
	vec3  diffuse;       
	vec3  specular;      
	float shininess;     
};

struct Texture {
	bool  useTexture;  

    sampler2D diffuse;
    sampler2D specular;
}; 

struct DirectionalLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
	bool turnedOn;

    vec3 position;
    
	// attenuation
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
	bool turnedOn;

    vec3 position;
    vec3 direction;

	// spotlight cone
    float cutOff;
    float outerCutOff;

	// attenuation  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in vec3 position_v;
in vec3 normal_v;
in vec2 textureCoords_v;
smooth in float fogFactor_v;

uniform vec3 viewPosition;
uniform float time;			
uniform float fog;

uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Material material;
uniform Texture textureMaterial;

out vec4 color_v;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 position_v, vec3 viewDirection);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 position_v, vec3 viewDirection);

void main() {    
    vec3 normal = normalize(normal_v);
    vec3 viewDirection = normalize(viewPosition - position_v);
    
    //	directional lighting
    vec3 result = CalculateDirectionalLight(directionalLight, normal, viewDirection);

    //	point light
//	if (pointLight.turnedOn == true)
// TODO: set one lamp in scene for point light
	result += CalculatePointLight(pointLight, normal, position_v, viewDirection);     
    
	//	spot light
//	if (spotLight.turnedOn == true)
	result += CalculateSpotLight(spotLight, normal, position_v, viewDirection);    
    
    color_v = vec4(result, 1.0);
	color_v = mix(color_v, vec4(0.6f,0.5f,0.4f,0.3f), fogFactor_v*fog);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
	light.direction = (vec4(-sin(time * 0.2f), -1.0f, -cos(time * 0.2f), 0.0)).xyz;
    vec3 lightDirection = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);

    // specular shading
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec			  = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	if (textureMaterial.useTexture) {
		ambient  = light.ambient * vec3(texture(textureMaterial.diffuse, textureCoords_v));
		diffuse  = light.diffuse * diff * vec3(texture(textureMaterial.diffuse, textureCoords_v));
		specular = light.specular * spec * vec3(texture(textureMaterial.specular, textureCoords_v));
	} else {
		ambient  = light.ambient * material.ambient;
		diffuse  = light.diffuse * diff * material.diffuse;
		specular = light.specular * spec * material.specular;
	}

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 position_v, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - position_v);

    // diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);

    // specular shading
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

    // attenuation
    float distance	  = length(light.position - position_v);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	if (textureMaterial.useTexture) {
		ambient  = light.ambient * vec3(texture(textureMaterial.diffuse, textureCoords_v));
		diffuse  = light.diffuse * diff * vec3(texture(textureMaterial.diffuse, textureCoords_v));
		specular = light.specular * spec * vec3(texture(textureMaterial.specular, textureCoords_v));
	} else {
		ambient  = light.ambient * material.ambient;
		diffuse  = light.diffuse * diff * material.diffuse;
		specular = light.specular * spec * material.specular;
	}

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
	return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 position_v, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - position_v);

    // diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);

    // specular shading
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec			  = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    
	// attenuation
    float distance    = length(light.position - position_v);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// spotlight intensity
    float theta     = dot(lightDirection, normalize(-light.direction)); 
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	if (textureMaterial.useTexture) {
		ambient  = light.ambient * vec3(texture(textureMaterial.diffuse, textureCoords_v));
		diffuse  = light.diffuse * diff * vec3(texture(textureMaterial.diffuse, textureCoords_v));
		specular = light.specular * spec * vec3(texture(textureMaterial.specular, textureCoords_v));
	} else {
		ambient  = light.ambient * material.ambient;
		diffuse  = light.diffuse * diff * material.diffuse;
		specular = light.specular * spec * material.specular;
	}

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
	return (ambient + diffuse + specular);
}