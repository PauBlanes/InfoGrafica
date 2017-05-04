#version 330 core

#define NUM_MAX_PLIGHTS 2
#define NUM_MAX_SLIGHTS 2

struct Material {
	 sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};  

struct PLight {
	vec3 position;  
  
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
	
	//las atenuaciones
    float constant;
    float linear;
    float quadratic;
};

struct DLight {
	vec3 direction;
	
	vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
};

struct SLight {
	vec3 position;
    vec3 direction;
    float maxApertura;
	float minApertura;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;  
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
out vec4 color;
  
uniform vec3 viewPos;
uniform Material material;
uniform DLight dlight;
uniform PLight plight[NUM_MAX_PLIGHTS];
uniform SLight slight[NUM_MAX_SLIGHTS];

vec3 DirectionalLight(DLight light, vec3 Normal, vec3 viewDirection);
vec3 PointLight(PLight light, vec3 Normal, vec3 viewDirection);
vec3 SpotLight(SLight light, vec3 Normal, vec3 viewDirection);

void main(){
	vec3 viewDir = normalize(viewPos - FragPos);
	
	vec3 final;
	
	final = DirectionalLight(dlight,Normal,viewDir);
	//los point lights
	for (int i = 0;i < NUM_MAX_PLIGHTS;i++)
		final += PointLight (plight[i], Normal, viewDir);
	//los spotlights
	for (int i = 0; i< NUM_MAX_SLIGHTS;i++)
		final += SpotLight(slight[i], Normal, viewDir);
	
	color = vec4 (final, 1.0f);
		
	
} 

vec3 DirectionalLight(DLight light, vec3 Normal, vec3 viewDirection){
	// Ambient
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * light.ambientColor * vec3(texture(material.diffuse, TexCoords));

    // Diffuse 
    vec3 normedNorm = normalize(Normal);
	float diff = max(dot(normedNorm, -light.direction), 0.0); //el cos entre vector llum i la normal.com que els tenim normalitzats i llavors hipotenusa es 1, no volem que sigui negatiu pq fa efectes raros
    vec3 diffuse = diff * light.diffuseColor * vec3(texture(material.diffuse, TexCoords));
	
	//Specular
	float specularStrength = 0.5f;
	vec3 reflectDir = reflect(light.direction, normedNorm); //negatiu el lightDir pq la funcio reflect l'espera en l'altre direccio
	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess); //elevem el cosino entre el reflex i la camera. el numero que elevem es lo concentrat que esta el reflex
	vec3 specular = specularStrength * spec * light.specularColor * vec3(texture(material.specular, TexCoords));
	
	//Final
    vec3 finalColor = (ambient + diffuse + specular);   	
	return finalColor;
}

vec3 PointLight(PLight light, vec3 Normal, vec3 viewDirection){
	// Ambient
    float ambientStrength = 0.3f;
    vec3 ambient = ambientStrength * light.ambientColor * vec3(texture(material.diffuse, TexCoords));

    // Diffuse 
    vec3 normedNorm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos); 
    float diff = max(dot(normedNorm, lightDir), 0.0); //el cos entre vector llum i la normal.com que els tenim normalitzats i llavors hipotenusa es 1, no volem que sigui negatiu pq fa efectes raros
    vec3 diffuse = diff * light.diffuseColor * vec3(texture(material.diffuse, TexCoords));
	
	//Specular
	float specularStrength = 0.9f;	 
	vec3 reflectDir = reflect(-lightDir, normedNorm); //negatiu el lightDir pq la funcio reflect l'espera en l'altre direccio
	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess); //elevem el cosino entre el reflex i la camera. el numero que elevem es lo concentrat que esta el reflex
	vec3 specular = specularStrength * spec * light.specularColor * vec3(texture(material.specular, TexCoords));
	
	//Atenuation
	float d = length(light.position - FragPos);
	float fatt = 1.0f / (light.constant+light.linear*d+(light.quadratic*pow(d,2)));
	
	//Final
    vec3 finalColor = fatt* vec3(ambient + diffuse + specular);
    return finalColor;
}

vec3 SpotLight(SLight light, vec3 Normal, vec3 viewDirection){
	    
    //ahora comprovar si esta dentro del foco
	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir,normalize(-light.direction));
	
	if (theta > light.maxApertura) { //estara dentro
		// Ambient
        float ambientStrength = 0.3f;
		vec3 ambient = ambientStrength* light.ambientColor * vec3(texture(material.diffuse, TexCoords));
        
        // Diffuse 
        vec3 norm = normalize(Normal);        
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuseColor * diff * vec3(texture(material.diffuse, TexCoords));  
        
        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
        vec3 specular = light.specularColor * spec * vec3(texture(material.diffuse, TexCoords));
        
		//intensidad dentro
		float epsilon   = light.minApertura - light.maxApertura;
		float intensity = clamp((theta - light.maxApertura) / epsilon, 0.0, 1.0); 
		diffuse  *= intensity;
		specular *= intensity;

        // Attenuation
        float d  = length(light.position - FragPos);
        float att = 1.0f / (1+0.09*d+(0.032*pow(d,2)));    
		ambient *= att;
        diffuse  *= att;
        specular *= att;   
                
        return (ambient + diffuse + specular);
		
	}
	else {
		 float ambientStrength = 0.3f;
		vec3 ambient = ambientStrength * light.ambientColor * vec3(texture(material.diffuse, TexCoords));
		float d  = length(light.position - FragPos);
        float att = 1.0f / (light.constant+light.linear*d+(light.quadratic*pow(d,2)));    
		ambient *= att;
		return ambient;
	}
	
}