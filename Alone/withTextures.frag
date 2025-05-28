#version 140

struct Material {
  sampler2D  diffuseSampler;            
  sampler2D  specularSampler;
  
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

in vec3 position_f;
in vec3 normals_f;
in vec3 textureCoords_f;

out vec4 color_f;

uniform vec3 cameraPosition;

void main() {
	// ambient component
	vec3 ambient = light.ambient * texture(material.diffuseSampler, textureCoords_f).rgb;

	// diffuse component
	vec3 normalazedNormals  = normalize(normals_f);
	vec3 lightDirection		= normalize(light.position - position_f);
	float diff				= max(dot(normalazedNormals, lightDirection), 0.0);
	vec3 diffuse			= light.diffuse * diff * texture(material.diffuseSampler, textureCoords_f).rgb;

	// specular component
	vec3 viewDirection		= normalize(cameraPosition - position_f);
	vec3 reflectorDirection = reflect(-lightDirection, normalazedNormals);
	float spec				= pow(max(dot(viewDirection, reflectorDirection),0.0),material.shininess);
	vec3 specular			= light.specular * spec * texture(material.specularSampler, textureCoords_f).rgb;

	// result material
	vec3 result = ambient + diffuse + specular;
	color_f		= vec4(result, 1.0);
}