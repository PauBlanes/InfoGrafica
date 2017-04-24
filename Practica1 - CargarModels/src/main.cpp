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
#include "Model.h"
#include "Object.h"

using namespace std;
using namespace glm;
const GLint WIDTH = 800, HEIGHT = 800;
bool WIREFRAME = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xScroll, double yScroll);

//inicializo camara
Camera myCamera (vec3(0, 0, 3), vec3(0, 0, 0), 0.04f, 45.0f);

//para mover el cubo
vec3 cubeMov(0.0f, -0.5f, -1.5f);
GLfloat cubeRot = 0;

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
	Shader lampShader ("./src/lampVertexShader.vertexshader", "./src/lampFragmentShader.fragmentshader");
	Shader simpleShader("./src/SimpleVertexShader.vertexshader", "./src/SimpleFragmentShader.fragmentshader");
	

	//Creamos los objectos
	Object lamp(vec3(0.1f, 0.1f, 0.1f), vec3(0.f, 0.f, 0.f), vec3(-0.0f, 0.7f, -2.0f)/*el tipo de figura*/);
	Object cubo(vec3(0.4f, 0.4f, 0.4f), vec3(0.f, 0.f, 0.f), vec3(0.0f, -0.5f, -1.0f)/*el tipo de figura*/);

	//bucle de dibujado
	while (!glfwWindowShouldClose(window))
	{
		
		//Activar culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
			
		//limpiar
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
		//moure
		myCamera.DoMovement(window);
		
		simpleShader.USE();

		//pasamos los uniforms para la luz
		GLint objectCol = glGetUniformLocation(simpleShader.Program, "objectColor");
		GLint lightCol = glGetUniformLocation(simpleShader.Program, "lightColor");
		glUniform3f(objectCol, 0.5f, 1.0f, 0.31f);
		glUniform3f(lightCol, 1.0f, 1.0f, 1.0f); // el color de la luz que es blanca
		GLint lightPosVar = glGetUniformLocation(simpleShader.Program, "lampPos");
		glUniform3f(lightPosVar, -0.0f, 0.3f, -2.0f);
		GLint viewPosVar = glGetUniformLocation(simpleShader.Program, "viewPos");
		glUniform3f(viewPosVar, myCamera.GetPos().x, myCamera.GetPos().y, myCamera.GetPos().z);

		// Las matrices
		mat4 model;
		mat4 view;
		mat4 projection;

		cubo.Move(cubeMov);
		cubo.Rotate(vec3 (0.f,1.f,0.f),cubeRot);
		model = cubo.GetModelMatrix(); 
		view = myCamera.LookAt();
		projection = perspective(myCamera.GetFOV(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		//lo pasamos al shader
		GLint modelMatVar = glGetUniformLocation(simpleShader.Program, "model");
		GLint viewMatVar = glGetUniformLocation(simpleShader.Program, "view");
		GLint projMatVar = glGetUniformLocation(simpleShader.Program, "projection");
		glUniformMatrix4fv(viewMatVar, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projMatVar, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(modelMatVar, 1, GL_FALSE, value_ptr(model));
		//Pintamos el cubo		
		cubo.Draw();
		
		//Usamos el shader del cubo lampara
		lampShader.USE();
		// Las matrices
		model = lamp.GetModelMatrix();
		//las pasamos al shader
		modelMatVar = glGetUniformLocation(lampShader.Program, "model");
		viewMatVar = glGetUniformLocation(lampShader.Program, "view");
		projMatVar = glGetUniformLocation(lampShader.Program, "projection");
		glUniformMatrix4fv(viewMatVar, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projMatVar, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(modelMatVar, 1, GL_FALSE, value_ptr(model));
		//Pintamos ya el cubo lampara
		lamp.Draw();
		
		//eventos
		glfwPollEvents();

		//desactivar culling
		glDisable(GL_CULL_FACE);

		// Swappear buffer
		glfwSwapBuffers(window);
		
	}
	
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_KP_8)
		cubeMov.y += 0.1f;
	if (key == GLFW_KEY_KP_2)
		cubeMov.y -= 0.1f;
	if (key == GLFW_KEY_KP_6)
		cubeMov.x += 0.1f;
	if (key == GLFW_KEY_KP_4)
		cubeMov.x -= 0.1f;
	if (key == GLFW_KEY_KP_9)
		cubeRot += 3.f;
	if (key == GLFW_KEY_KP_7)
		cubeRot -= 3.f;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseMove(window, xpos, ypos);
}
void scroll_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseScroll(window, xpos, ypos);
}