//GLEW
#define GLEW_STATIC
#include <GL\glew.h>
//GLFW
#include <GLFW\glfw3.h>
#include <iostream>
#include "myShader.h"
#include "SOIL.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Camera.h"

using namespace std;
using namespace glm;
const GLint WIDTH = 800, HEIGHT = 800;
bool WIREFRAME = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//void DrawVAO();
mat4 GenerateModelMatrix(bool automatico, vec3 startingPos);
//void doMovement(GLFWwindow* window);
//para que el callback no de problemas llamo a las funciones de la classe aqui dentro
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xScroll, double yScroll);

//ejercicio rotar y textura
GLfloat tantoXCiento;
GLfloat Yrot;
GLfloat Xrot;

//inicializo camara
Camera myCamera (vec3(0, 0, 3), vec3(0, 0, 0), 0.04f, 45.0f);

int main() {
	
	//initGLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);
	
	//set GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//create a window
	GLFWwindow* window;
	window = glfwCreateWindow(WIDTH, HEIGHT, "Magnifica ventana que tenemos aqui", nullptr, nullptr);
	if (!window) {
		cout << "Error al crear la ventana" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	//set GLEW and inicializate
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		cout << "Error al inicializar glew" << endl;
		glfwTerminate();
		return NULL;
	}

	//set function when callback
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set windows and viewport
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	//empezamos el z buffer
	glEnable(GL_DEPTH_TEST);
	
	//cargamos los shader
	Shader myShader ("./src/3DVertex.vertexshader", "./src/3DFragment.fragmentshader");	

	//VAOVBOANTIGUOS
	/*
	GLfloat VertexBufferCube[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	
	vec3 CubesPositionBuffer[] = { //posiciones de los cubos
		vec3(0.0f ,  0.0f,  0.0f),
		vec3(2.0f ,  5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3(2.4f , -0.4f, -3.5f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3(1.3f , -2.0f, -2.5f),
		vec3(1.5f ,  2.0f, -2.5f),
		vec3(1.5f ,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f)
	};
			
	// Crear los VBO, VAO y EBO	//Declarar el VBO y el EBO	
	GLuint EBO, VBO, VAO;
	//reservar memoria para el VAO, VBO y EBO
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	//Establecer el objeto
	glBindVertexArray(VAO);{
		//Enlazar el buffer con openGL
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBufferCube), VertexBufferCube, GL_STATIC_DRAW);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubesPositionBuffer), CubesPositionBuffer, GL_STATIC_DRAW);

		//Establecer las propiedades de los vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)0); 
		glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GLfloat)));
		//glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		//liberar el buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	} 
	//liberar el buffer de vertices
	glBindVertexArray(0);

	//crear textura1
	GLuint texture1ID;
	glGenTextures(1, &texture1ID);
	glBindTexture(GL_TEXTURE_2D, texture1ID);
	//Wrapping y filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//imagen
	int width, height;
	unsigned char* image = SOIL_load_image("./src/texture.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	//crear textura2
	GLuint texture2ID;
	glGenTextures(1, &texture2ID);
	glBindTexture(GL_TEXTURE_2D, texture2ID);
	//Wrapping y filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//imagen
	int width2, height2;
	unsigned char* image2 = SOIL_load_image("./src/dietaSana.png", &width2, &height2, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	SOIL_free_image_data(image2);
	glBindTexture(GL_TEXTURE_2D, 0);
	*/	

	//bucle de dibujado
	while (!glfwWindowShouldClose(window))
	{
			
			//limpiar
			glClearColor(1.f, 1.f, 1.f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			/*
			// Bind Textures using texture units
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1ID);
			glUniform1i(glGetUniformLocation(myShader.Program, "Texture1"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2ID);
			glUniform1i(glGetUniformLocation(myShader.Program, "Texture2"), 1);
			*/

			//Activamos shader
			myShader.USE();

			//la textura
			GLfloat variableShader = glGetUniformLocation(myShader.Program, "factor");
			glUniform1f(variableShader, tantoXCiento);


			//moure
			myCamera.DoMovement(window);

			// Las matrices
			mat4 model;
			mat4 view;
			mat4 projection;
			
			view = myCamera.LookAt();		
			projection = perspective(myCamera.GetFOV(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f); 
			//lo pasamos al shader
			GLint viewLoc = glGetUniformLocation(myShader.Program, "view");
			GLint projLoc = glGetUniformLocation(myShader.Program, "projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

			/*
			//los 10 cubos sus matrices de modelo
			for (int i = 0; i < 10;i++) {
				//la matriz de modelo
				if (i == 0)
					model = GenerateModelMatrix(false, CubesPositionBuffer[i]);
				else
					model = GenerateModelMatrix(true, CubesPositionBuffer[i]);

				// Lo pasamos al shader
				GLint modelLoc = glGetUniformLocation(myShader.Program, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

				// Pintar
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
			}

			*/
			//eventos
			glfwPollEvents();

			// Swappear buffer
			glfwSwapBuffers(window);
		
	}
	/*
	//liberar memoria texturas
	glDeleteTextures(1, &texture1ID);
	glDeleteTextures(1, &texture2ID);
	// liberar la memoria de los VAO, EBO y VBO
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	*/
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	exit(EXIT_SUCCESS);
}



mat4 GenerateModelMatrix(bool automatico, vec3 startingPos) {
	mat4 temp;
	temp = translate(temp, startingPos);
	if (!automatico) {
		temp = rotate(temp, radians(Xrot), vec3(0.f, 1.f, 0.0f));
		temp = rotate(temp, radians(Yrot), vec3(1.f, 0.f, 0.0f));
	}
	else {
		temp = rotate(temp, (GLfloat)glfwGetTime() * 0.1f, vec3(0.f, 1.f, 0.0f));
		temp = rotate(temp, (GLfloat)glfwGetTime() * 0.1f, vec3(1.f, 0.f, 0.0f));
	}
	return temp;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);	
			
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseMove(window, xpos, ypos);
}
void scroll_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseScroll(window, xpos, ypos);
}