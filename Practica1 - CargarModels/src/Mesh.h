#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// GL Includes
#include <GL\glew.h> 
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "myShader.h"
#include <assimp\types.h>

using namespace std;
using namespace glm;

struct Vertex {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
};

struct Texture {
	GLuint id;
	string type;
	aiString path;
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	void Draw(Shader MeshShader, GLint DrawMode);

private:	
	GLuint VAO, VBO, EBO;
	void setupMesh();
};