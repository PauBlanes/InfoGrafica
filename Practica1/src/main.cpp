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
#include "material.h"
#include "Light.h"

using namespace std;
using namespace glm;
const GLint WIDTH = 800, HEIGHT = 800;
bool WIREFRAME = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xScroll, double yScroll);

//inicializo camara
Camera myCamera(vec3(0, 0, 3), vec3(0, 0, 0), 0.04f, 45.0f);

//para mover el cubo
vec3 cubeMov(0.0f, -0.5f, -1.5f);
GLfloat cubeRot = 0;

//activar/descativar linterna
bool lantern = false;

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
	Shader lampShader("./src/lampVertexShader.vertexshader", "./src/lampFragmentShader.fragmentshader");
	Shader dirShader("./src/DirLightVertexShader.vertexshader", "./src/DirLightFragmentShader.fragmentshader");
	Shader pointShader("./src/PointLightVertexShader.vertexshader", "./src/PointLightFragmentShader.fragmentshader");
	Shader focalShader("./src/FocalLightVertexShader.vertexshader", "./src/FocalLightFragmentShader.fragmentshader");
	Shader fullShader("./src/VertexShaderPhongTexture.vs", "./src/FragmentShaderPhongTexture.fs");


	//Creamos materiales
	Material mat1("./src/Materials/difuso.png", "./src/Materials/especular.png", 32);
	mat1.ActivateTextures();

	//Creamos luces
	Light pointLight1(vec3(-0.0f, 0.7f, -1.0f), vec3(0), vec3(.8f, 0.8f, 0.8f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), Light::POINT, 0);
	pointLight1.SetAtt(1, 0.09, 0.032);
	Light pointLight2(vec3(5.f, 0.7f, -1.0f), vec3(0), vec3(.8f, 0.8f, 0.8f), vec3(0.f, 1.0f, 0.f), vec3(0.f, 1.0f, 0.f), Light::POINT, 1);
	pointLight2.SetAtt(1, 0.09, 0.032);
	Light sLight1(vec3(10.f, 0.7f, -1.5f), vec3(0, -1, 0), vec3(.8f, 0.8f, 0.8f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), Light::SPOT, 0);
	sLight1.SetAtt(1, 0.09, 0.032);
	sLight1.SetAperture(cos(radians(10.f)), cos(radians(12.5f)));
	Light sLight2(vec3(0.0f, -0.5f, 0.f), vec3(0, 0, -1), vec3(.8f, 0.8f, 0.8f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), Light::SPOT, 1);
	sLight2.SetAtt(1, 0.09, 0.032);
	sLight2.SetAperture(cos(radians(5.5f)), cos(radians(7.f)));
	Light dLight1(vec3(0), vec3(0, -1, 0), vec3(0.f, 0.f, 1.f), vec3(0.f, 0.f, 1.f), vec3(0.f, 0.f, 1.f), Light::DIRECTIONAL, 0);

	//Creamos los objectos
	Object lamp1(vec3(0.1f, 0.1f, 0.1f), vec3(0.f, 0.f, 0.f), vec3(-0.0f, 0.7f, -1.0f)/*el tipo de figura*/); //es Object::TIPO
	Object lamp2(vec3(0.1f, 0.1f, 0.1f), vec3(0.f, 0.f, 0.f), vec3(5.f, 0.7f, -1.0f)/*el tipo de figura*/); //es Object::TIPO
	Object lamp3(vec3(0.1f, 0.1f, 0.1f), vec3(0.f, 0.f, 0.f), vec3(10.f, 0.7f, -1.5f)/*el tipo de figura*/); //es Object::TIPO
	Object cubo(vec3(0.8f, 0.8f, 0.8f), vec3(0.f, 0.f, 0.f), vec3(0.0f, -0.5f, -1.0f)/*el tipo de figura*/);



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


		fullShader.USE();

		//uniforms para los materiales
		mat1.SetMaterial(&fullShader);
		mat1.SetShininess(&fullShader);


		//Activar luz
		dLight1.SetLight(&fullShader, myCamera.GetPos());
		pointLight1.SetLight(&fullShader, myCamera.GetPos());
		pointLight2.SetLight(&fullShader, myCamera.GetPos());
		sLight1.SetLight(&fullShader, myCamera.GetPos());
		//la linterna
		sLight2.SetLight(&fullShader, myCamera.GetPos());
		if (lantern) {
			sLight2.SetPosition(myCamera.GetPos());
			sLight2.SetDirection(myCamera.GetDir());
		}
		// Las matrices
		mat4 model;
		mat4 view;
		mat4 projection;

		cubo.Move(cubeMov);
		cubo.Rotate(vec3(1.f, 0.f, 0.f), cubeRot);
		model = cubo.GetModelMatrix();
		view = myCamera.LookAt();
		projection = perspective(myCamera.GetFOV(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		//lo pasamos al shader
		GLint modelMatVar = glGetUniformLocation(fullShader.Program, "model");
		GLint viewMatVar = glGetUniformLocation(fullShader.Program, "view");
		GLint projMatVar = glGetUniformLocation(fullShader.Program, "projection");
		glUniformMatrix4fv(viewMatVar, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projMatVar, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(modelMatVar, 1, GL_FALSE, value_ptr(model));
		//Pintamos el cubo		
		cubo.Draw();

		//LAMPARA1
		lampShader.USE();
		//Colores luz
		lamp1.ChangeColor(pointLight1.GetColor(), &lampShader);
		// Las matrices
		model = lamp1.GetModelMatrix();
		//las pasamos al shader
		modelMatVar = glGetUniformLocation(lampShader.Program, "model");
		viewMatVar = glGetUniformLocation(lampShader.Program, "view");
		projMatVar = glGetUniformLocation(lampShader.Program, "projection");
		glUniformMatrix4fv(viewMatVar, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projMatVar, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(modelMatVar, 1, GL_FALSE, value_ptr(model));
		//Pintamos ya el cubo lampara
		lamp1.Draw();
		//LAMPARA2
		//Colores luz
		lamp2.ChangeColor(pointLight2.GetColor(), &lampShader);
		// Las matrices
		model = lamp2.GetModelMatrix();
		//las pasamos al shader
		modelMatVar = glGetUniformLocation(lampShader.Program, "model");
		viewMatVar = glGetUniformLocation(lampShader.Program, "view");
		projMatVar = glGetUniformLocation(lampShader.Program, "projection");
		glUniformMatrix4fv(viewMatVar, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projMatVar, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(modelMatVar, 1, GL_FALSE, value_ptr(model));
		//Pintamos ya el cubo lampara
		lamp2.Draw();
		//LAMPARA3
		//Colores luz
		lamp3.ChangeColor(sLight1.GetColor(), &lampShader);
		// Las matrices
		model = lamp3.GetModelMatrix();
		//las pasamos al shader
		modelMatVar = glGetUniformLocation(lampShader.Program, "model");
		viewMatVar = glGetUniformLocation(lampShader.Program, "view");
		projMatVar = glGetUniformLocation(lampShader.Program, "projection");
		glUniformMatrix4fv(viewMatVar, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projMatVar, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(modelMatVar, 1, GL_FALSE, value_ptr(model));
		//Pintamos ya el cubo lampara
		lamp3.Draw();

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
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		if (!lantern)
			lantern = true;
		else
			lantern = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseMove(window, xpos, ypos);
}
void scroll_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseScroll(window, xpos, ypos);
}