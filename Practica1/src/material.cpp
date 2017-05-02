#include "material.h"

Material::Material(char *DiffPath, char*SpectPath, float Shini){
	Shininess = Shini;

	//crear textura1
	glGenTextures(1, &TextDiff);
	glBindTexture(GL_TEXTURE_2D, TextDiff);
	//Wrapping y filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //abans era GL_CLAMP TO EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//imagen
	int width, height;
	unsigned char* image = SOIL_load_image(DiffPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	//crear textura2
	glGenTextures(1, &TextSpec);
	glBindTexture(GL_TEXTURE_2D, TextSpec);
	//Wrapping y filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//imagen
	int width2, height2;
	unsigned char* image2 = SOIL_load_image(SpectPath, &width2, &height2, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	SOIL_free_image_data(image2);
	glBindTexture(GL_TEXTURE_2D, 0);
}


Material::~Material(){
	glDeleteTextures(1, &TextDiff);
	glDeleteTextures(1, &TextSpec);
}

void Material::SetMaterial(Shader *shad) {

	glUniform1i(glGetUniformLocation(shad->Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shad->Program, "material.specular"), 1);
}

void Material::SetShininess(Shader *shad) {
	GLfloat shinyVar = glGetUniformLocation(shad->Program, "material.shininess");
	glUniform1f(shinyVar, Shininess);
}

void Material::ActivateTextures() {
	//poner cual textura es cada una
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextDiff);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextSpec);
}
