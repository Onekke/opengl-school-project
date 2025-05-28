#version 140

struct Material {
  vec3  ambient;        
  vec3  diffuse;            
  vec3  specular;
  
  float shininess;          
};
uniform Material material;

struct Light {
	vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct PointLight {
	vec3 position;
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;
};
uniform PointLight point;

smooth in float fogFactor_v; 
in vec3 position_f;
in vec3 normals_f;

out vec4 color_f;

uniform vec3 cameraPosition;

void main() {
	// directional light
	// ambient component
	vec3 ambient = light.ambient * material.ambient;

	// diffuse component, diffuse shading
	vec3 normalazedNormals  = normalize(normals_f);
	vec3 lightDirection		= normalize(light.position - position_f);
	float diff				= max(dot(normalazedNormals, lightDirection), 0.0);
	vec3 diffuse			= light.diffuse * (diff * material.diffuse);

	// specular component, specular shading
	vec3 viewDirection		= normalize(cameraPosition - position_f);
	vec3 reflectorDirection = reflect(-lightDirection, normalazedNormals);
	float spec				= pow(max(dot(viewDirection,reflectorDirection),0.0),material.shininess);
	vec3 specular			= light.specular * (spec * material.specular);

	// point light
	vec3 pointDirection			 = normalize(point.direction);
	float pointDiff				 = max(dot(normalazedNormals, pointDirection), 0.0);
	vec3 pointReflectorDirection = reflect(-pointDirection, normalazedNormals);
	float pointSpec				 = pow(max(dot(viewDirection,pointReflectorDirection),0.0),material.shininess);

	// attenuation
	float distance = length(point.position - position_f);
	float attenuation = 1.0/(point.constant + point.linear * distance + point.quadratic * (distance * distance));

	vec3 pointAmbient  = point.ambient * material.ambient * attenuation;
	vec3 pointDiffuse  = point.diffuse * (pointDiff * material.diffuse) * attenuation;
	vec3 pointSpecular = point.specular * (pointSpec * material.specular) * attenuation;

	// result material
	vec3 result = ambient + diffuse + specular + pointAmbient + pointDiffuse + pointSpecular;

	color_f		= vec4(result, 1.0);
//	vec4 fogColor = vec4(result, 1.0);
//	color_f = mix(color_f, fogColor, fogFactor_v);
}