#version 330 core
out vec4 FragColor;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
  
uniform vec3 viewPos;
uniform Material material;
uniform Material material2;
uniform Light lights[2];

uniform bool selected = false;

void main()
{
    // ambient
	vec4 ambient;
    if (selected) {
	    //ambient = light.ambient * (material.ambient + vec3(0.6, 0.6, 0.5));
		//ambient = (lights[0].ambient + lights[1].ambient) * material2.ambient;
		ambient = lights[0].ambient * material2.ambient;
	}
	else {
	    //ambient = (lights[0].ambient + lights[1].ambient) * material.ambient;
		ambient = lights[0].ambient * material.ambient;
	}
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir0 = normalize(lights[0].position - FragPos);
	//vec3 lightDir1 = normalize(lights[1].position - FragPos);
    float diff0 = max(dot(norm, lightDir0), 0.0);
	//float diff1 = max(dot(norm, lightDir1), 0.0);
    vec4 diffuse0 = lights[0].diffuse * (diff0 * material.diffuse);
	//vec4 diffuse1 = lights[1].diffuse * (diff1 * material.diffuse);
	//vec4 diffuse = diffuse0 + diffuse1;
	vec4 diffuse = diffuse0;

    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir0 = reflect(-lightDir0, norm);
	//vec3 reflectDir1 = reflect(-lightDir1, norm);
    float spec0 = pow(max(dot(viewDir, reflectDir0), 0.0), material.shininess);
	//float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), material.shininess);

	vec4 specular0, specular1;
	if (selected) {
		specular0 = lights[0].specular * (spec0 * material2.specular);
		//specular1 = lights[1].specular * (spec1 * material2.specular);
	}
	else {
	    specular0 = lights[0].specular * (spec0 * material.specular);
		//specular1 = lights[1].specular * (spec1 * material.specular);
	}
	//vec4 specular = specular0 + specular1;
	vec4 specular = specular0;
        
    vec4 result = ambient + diffuse + specular;
    //FragColor = vec4(result, 1.0);
	FragColor = result;
}