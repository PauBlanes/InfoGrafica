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

//para cambiar luces
int lightType = 1;

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
	Shader dirShader("./src/DirLightVertexShader.vertexshader", "./src/DirLightFragmentShader.fragmentshader");
	Shader pointShader("./src/PointLightVertexShader.vertexshader", "./src/PointLightFragmentShader.fragmentshader");
	Shader focalShader("./src/FocalLightVertexShader.vertexshader", "./src/FocalLightFragmentShader.fragmentshader");
	

	//Creamos los objectos
	Object lamp(vec3(0.1f, 0.1f, 0.1f), vec3(0.f, 0.f, 0.f), vec3(-0.0f, 0.7f, -1.0f)/*el tipo de figura*/);
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
		if (lightType == 1) {
			pointShader.USE();

			//pasamos los uniforms para la luz
			GLint objectCol = glGetUniformLocation(pointShader.Program, "objectColor");
			GLint ambCol = glGetUniformLocation(pointShader.Program, "ambientColor");
			GLint diffCol = glGetUniformLocation(pointShader.Program, "diffuseColor");
			GLint specCol = glGetUniformLocation(pointShader.Program, "specularColor");
			glUniform3f(objectCol, 0.372549, 0.619608, 0.627451);
			glUniform3f(ambCol, .8f, 0.8f, 0.8f);
			glUniform3f(diffCol, 1.0f, 1.0f, 1.0f);
			glUniform3f(specCol, 1.0f, 1.0f, 1.0f);
			GLint viewPosVar = glGetUniformLocation(pointShader.Program, "viewPos");
			glUniform3f(viewPosVar, myCamera.GetPos().x, myCamera.GetPos().y, myCamera.GetPos().z);
			GLint lightPosVar = glGetUniformLocation(pointShader.Program, "lampPos");
			glUniform3f(lightPosVar, -0.0f, 0.3f, -2.0f);
			GLint DirLuzVar = glGetUniformLocation(pointShader.Program, "lightDir");
			glUniform3f(DirLuzVar, 0.0f, -1.0f, 0.0f);

			// Las matrices
			mat4 model;
			mat4 view;
			mat4 projection;

			cubo.Move(cubeMov);
			cubo.Rotate(vec3(0.f, 1.f, 0.f), cubeRot);
			model = cubo.GetModelMatrix();
			view = myCamera.LookAt();
			projection = perspective(myCamera.GetFOV(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
			//lo pasamos al shader
			GLint modelMatVar = glGetUniformLocation(pointShader.Program, "model");
			GLint viewMatVar = glGetUniformLocation(pointShader.Program, "view");
			GLint projMatVar = glGetUniformLocation(pointShader.Program, "projection");
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
		}
		if (lightType == 2) {
			dirShader.USE();

			//pasamos los uniforms para la luz
			GLint objectCol = glGetUniformLocation(dirShader.Program, "objectColor");
			GLint ambCol = glGetUniformLocation(dirShader.Program, "ambientColor");
			GLint diffCol = glGetUniformLocation(dirShader.Program, "diffuseColor");
			GLint specCol = glGetUniformLocation(dirShader.Program, "specularColor");
			glUniform3f(objectCol, 0.372549, 0.619608, 0.627451);
			glUniform3f(ambCol, .8f, 0.8f, 0.8f);
			glUniform3f(diffCol, 1.0f, 1.0f, 1.0f);
			glUniform3f(specCol, 1.0f, 1.0f, 1.0f);
			GLint viewPosVar = glGetUniformLocation(dirShader.Program, "viewPos");
			glUniform3f(viewPosVar, myCamera.GetPos().x, myCamera.GetPos().y, myCamera.GetPos().z);
			GLint DirLuzVar = glGetUniformLocation(dirShader.Program, "lightDir");
			glUniform3f(DirLuzVar, 0.0f, -1.0f, 0.0f);
						
			// Las matrices
			mat4 model;
			mat4 view;
			mat4 projection;

			cubo.Move(cubeMov);
			cubo.Rotate(vec3(0.f, 1.f, 0.f), cubeRot);
			model = cubo.GetModelMatrix();
			view = myCamera.LookAt();
			projection = perspective(myCamera.GetFOV(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
			//lo pasamos al shader
			GLint modelMatVar = glGetUniformLocation(dirShader.Program, "model");
			GLint viewMatVar = glGetUniformLocation(dirShader.Program, "view");
			GLint projMatVar = glGetUniformLocation(dirShader.Program, "projection");
			glUniformMatrix4fv(viewMatVar, 1, GL_FALSE, value_ptr(view));
			glUniformMatrix4fv(projMatVar, 1, GL_FALSE, value_ptr(projection));
			glUniformMatrix4fv(modelMatVar, 1, GL_FALSE, value_ptr(model));
			//Pintamos el cubo		
			cubo.Draw();
		}
		if (lightType == 3) {
			focalShader.USE();

			//pasamos los uniforms para la luz
			GLint objectCol = glGetUniformLocation(focalShader.Program, "objectColor");
			GLint ambCol = glGetUniformLocation(focalShader.Program, "ambientColor");
			GLint diffCol = glGetUniformLocation(focalShader.Program, "diffuseColor");
			GLint specCol = glGetUniformLocation(focalShader.Program, "specularColor");
			glUniform3f(objectCol, 0.372549, 0.619608, 0.627451);
			glUniform3f(ambCol, .8f, 0.8f, 0.8f);
			glUniform3f(diffCol, 1.0f, 1.0f, 1.0f);
			glUniform3f(specCol, 1.0f, 1.0f, 1.0f);
			GLint viewPosVar = glGetUniformLocation(focalShader.Program, "viewPos");
			glUniform3f(viewPosVar, myCamera.GetPos().x, myCamera.GetPos().y, myCamera.GetPos().z);
			GLint lightPosVar = glGetUniformLocation(focalShader.Program, "LightPos");
			glUniform3f(lightPosVar, 0, 0, 0);
			GLint AperturaMax = glGetUniformLocation(focalShader.Program, "cosAperturaMax");
			glUniform1f(AperturaMax, cos(radians(20.f)));
			GLint AperturaMin = glGetUniformLocation(focalShader.Program, "cosAperturaMin");
			glUniform1f(AperturaMin, cos(radians(12.f)));
			GLint DirLuzVar = glGetUniformLocation(focalShader.Program, "Fdir");
			glUniform3f(DirLuzVar, 0, 0, -1);

			// Las matrices
			mat4 model;
			mat4 view;
			mat4 projection;

			cubo.Move(cubeMov);
			cubo.Rotate(vec3(0.f, 1.f, 0.f), cubeRot);
			model = cubo.GetModelMatrix();
			view = myCamera.LookAt();
			projection = perspective(myCamera.GetFOV(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
			//lo pasamos al shader
			GLint modelMatVar = glGetUniformLocation(focalShader.Program, "model");
			GLint viewMatVar = glGetUniformLocation(focalShader.Program, "view");
			GLint projMatVar = glGetUniformLocation(focalShader.Program, "projection");
			glUniformMatrix4fv(viewMatVar, 1, GL_FALSE, value_ptr(view));
			glUniformMatrix4fv(projMatVar, 1, GL_FALSE, value_ptr(projection));
			glUniformMatrix4fv(modelMatVar, 1, GL_FALSE, value_ptr(model));
			//Pintamos el cubo		
			cubo.Draw();
		}		
		
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
	if (key == GLFW_KEY_KP_1)
		cubeMov.z += 0.1f;
	if (key == GLFW_KEY_KP_3)
		cubeMov.z -= 0.1f;
	if (key == GLFW_KEY_KP_9)
		cubeRot += 3.f;
	if (key == GLFW_KEY_KP_7)
		cubeRot -= 3.f;
	if (key == GLFW_KEY_1)
		lightType = 1;
	if (key == GLFW_KEY_2)
		lightType = 2;
	if (key == GLFW_KEY_3)
		lightType = 3;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseMove(window, xpos, ypos);
}
void scroll_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseScroll(window, xpos, ypos);
}