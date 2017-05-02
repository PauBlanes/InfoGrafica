#include "Light.h"

Light::Light(vec3 pos, vec3 dir, vec3 ambient, vec3 diffuse, vec3 specular, LType lType, int number) {
	switch (lType) {
	case DIRECTIONAL:
		Ldirection = dir;
		break;
	case POINT:
		Lpos = pos;
		break;
	case SPOT:
		Lpos = pos;
		Ldirection = dir;
		break;
	default:
		break;
	}

	Lambient = ambient;
	Ldiffuse = diffuse;
	Lspecular = specular;
	lightNumber = number; //el numero en el array de luces
	LightType = lType;
}


void Light::SetAtt(float constant, float lineal, float quadratic) {
	c1 = constant;
	c2 = lineal;
	c3 = quadratic;
}

void Light::SetAperture(float min, float max) {
	MinAperture = min;
	MaxAperture = max;
}

void Light::SetLight(Shader *shad, vec3 CamPos) {
	std::string variable; //la luz en sí

	//la pos de la camera
	glUniform3f(glGetUniformLocation(shad->Program, "viewPos"), CamPos.x, CamPos.y, CamPos.z);	
	 
	//Y variables ya diferentes para cada una
	switch (LightType){
	case DIRECTIONAL:
		variable = "dlight";
		//dirección
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".direction").c_str()), Ldirection.x, Ldirection.y, Ldirection.z);
		//colores
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".ambientColor").c_str()), Lambient.r, Lambient.g, Lambient.b);
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".diffuseColor").c_str()), Ldiffuse.r, Ldiffuse.g, Ldiffuse.b);
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".specularColor").c_str()), Lspecular.r, Lspecular.g, Lspecular.b);
		break;
	case POINT:		
		variable = "plight[" + std::to_string(lightNumber) + "]";
		//posicion
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".position").c_str()), Lpos.x, Lpos.y, Lpos.z);
		//colores
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".ambientColor").c_str()), Lambient.r, Lambient.g, Lambient.b);
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".diffuseColor").c_str()), Ldiffuse.r, Ldiffuse.g, Ldiffuse.b);
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".specularColor").c_str()), Lspecular.r, Lspecular.g, Lspecular.b);
		//atenauciones
		glUniform1f(glGetUniformLocation(shad->Program, (variable + ".constant").c_str()), c1);
		glUniform1f(glGetUniformLocation(shad->Program, (variable + ".linear").c_str()), c2);
		glUniform1f(glGetUniformLocation(shad->Program, (variable + ".quadratic").c_str()), c3);
		break;
	case SPOT:
		variable = "slight[" + std::to_string(lightNumber) + "]";
		//posicion
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".position").c_str()), Lpos.x, Lpos.y, Lpos.z);
		//direccion
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".direction").c_str()), Ldirection.x, Ldirection.y, Ldirection.z);
		//colores
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".ambientColor").c_str()), Lambient.r, Lambient.g, Lambient.b);
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".diffuseColor").c_str()), Ldiffuse.r, Ldiffuse.g, Ldiffuse.b);
		glUniform3f(glGetUniformLocation(shad->Program, (variable + ".specularColor").c_str()), Lspecular.r, Lspecular.g, Lspecular.b);
		//atenauciones
		glUniform1f(glGetUniformLocation(shad->Program, (variable + ".constant").c_str()), c1);
		glUniform1f(glGetUniformLocation(shad->Program, (variable + ".linear").c_str()), c2);
		glUniform1f(glGetUniformLocation(shad->Program, (variable + ".quadratic").c_str()), c3);
		//angulos de apertura
		glUniform1f(glGetUniformLocation(shad->Program, (variable + ".maxApertura").c_str()), MaxAperture);
		glUniform1f(glGetUniformLocation(shad->Program, (variable + ".minApertura").c_str()), MinAperture);

		break;
	default:
		break;
	}	
}

void Light::Rotate(vec3 rotation) {	
//opcional
}

void Light::SetDirection(vec3 dir) {
	Ldirection = dir;
}

void Light::SetPosition(vec3 pos) {
	Lpos = pos;
}

vec3 Light::GetColor() {
	vec3 temp = (Lambient + Ldiffuse + Lspecular);
	temp /= 3;
	return temp;
}