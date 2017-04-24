#pragma once
#include <GL\glew.h>
#include<glm.hpp>
#include <gtc\matrix_transform.hpp>
using namespace glm;
#include "myShader.h"

class Object{
	
public:
	enum FigureType{
		cube = 0,
	};

	Object(vec3 scl, vec3 rot, vec3 pos/*, FigureType typef*/);
	~Object();

	void Draw();
	void Move(vec3 translation);
	void Rotate(vec3 rota, GLfloat rotationAmount);
	void Scale(vec3 scal);
	void Delete();
	mat4 GetModelMatrix();
	vec3 GetPosition();

private:
	GLuint VBO, VAO, EBO;
	vec3 position;
	vec3 escala;
	vec3 rotationAxis;
	GLfloat rotation;
};

