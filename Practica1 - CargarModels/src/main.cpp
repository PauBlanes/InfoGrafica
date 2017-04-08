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

//para escojer el modelo que cargar
int whatToLoad = 1;

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

	//Modelos
	Model modelo1("./src/Models/spider/spider.obj");
	Model modelo2("./src/Models/Toilet/Toilet.obj");
	Model modelo3("./src/Models/tableandchair/tableandchair.obj");
	

	//bucle de dibujado
	while (!glfwWindowShouldClose(window))
	{
		
		//Activar culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
			
		//limpiar
		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
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

		model = translate(model, vec3(0.0f, 0.0f, -2.0f)); // Translate it down a bit so it's at the center of the scene
		model = scale(model, vec3(0.1f, 0.1f, 0.1f)); // Translate it down a bit so it's at the center of the scene	
		view = myCamera.LookAt();		
		projection = perspective(myCamera.GetFOV(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f); 
		//lo pasamos al shader
		GLint viewLoc = glGetUniformLocation(myShader.Program, "view");
		GLint projLoc = glGetUniformLocation(myShader.Program, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "model"), 1, GL_FALSE, value_ptr(model));
						
		//Pintar
		if (whatToLoad == 1)
			modelo1.Draw(myShader, GL_FILL);
		else if (whatToLoad == 2)
			modelo2.Draw(myShader, GL_FILL);
		else if (whatToLoad == 3)
			modelo3.Draw(myShader, GL_FILL);

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
	if (key == GLFW_KEY_1)
		whatToLoad = 1;
	if (key == GLFW_KEY_2)
		whatToLoad = 2;
	if (key == GLFW_KEY_3)
		whatToLoad = 3;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);	
			
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseMove(window, xpos, ypos);
}
void scroll_callback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseScroll(window, xpos, ypos);
}