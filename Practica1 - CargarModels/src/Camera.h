#pragma once
//GLEW
#define GLEW_STATIC
#include <GL\glew.h>
//GLFW
#include <GLFW\glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


using namespace std;
using namespace glm;


class Camera {
private:
	vec3 cameraPos;
	vec3 cameraFront;
	vec3 cameraUp;
	GLfloat cameraSpeed;
	GLfloat deltaTime;
	GLfloat lastFrame;
	GLfloat lastMx;
	GLfloat lastMy;
	GLfloat sensitivity;
	GLboolean firstMouse;
	GLfloat myPitch;
	GLfloat myYaw;
	GLfloat myFov;
public:
	Camera(vec3 position, vec3 target, GLfloat sens, GLfloat fov);
	void DoMovement(GLFWwindow * window);
	void MouseMove(GLFWwindow* window, double xpos, double ypos);
	void MouseScroll(GLFWwindow* window, double xScroll, double yScroll);
	mat4 LookAt();
	GLfloat GetFOV();
};
