#include "Camera.h"

Camera::Camera(vec3 position, vec3 target, GLfloat sens, GLfloat fov) {
	cameraPos = position;
	cameraFront = normalize(target - cameraPos);
	cameraUp = cross(cameraFront, cross(vec3(0, 1, 0), cameraFront));
	sensitivity = sens;
	myFov = fov;
	cameraSpeed = 3.f;
	firstMouse = true;
	myYaw = -90.0f; //pq sino da problemas
}

void Camera::DoMovement(GLFWwindow * window) {
	//calcular deltaTime
	GLfloat actualTime = glfwGetTime();
	deltaTime = actualTime - lastFrame;
	lastFrame = actualTime;
	
	if (glfwGetKey(window, GLFW_KEY_W))
		cameraPos += cameraSpeed * cameraFront * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S))
		cameraPos -= cameraSpeed * cameraFront * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A))
		cameraPos -= cross(cameraFront, cameraUp) * cameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D))
		cameraPos += cross(cameraFront, cameraUp) * cameraSpeed * deltaTime;
}

void Camera::MouseMove(GLFWwindow* window, double xpos, double ypos) {
	//si es la primera vez hacemos esto para que no salte de golpe
	if (firstMouse)
	{
		lastMx = xpos;
		lastMy = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastMx;
	GLfloat yoffset = lastMy - ypos;
	lastMx = xpos;
	lastMy = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	myYaw += xoffset;
	myPitch += yoffset;

	//dejar un maximo de 180 grados de libertad horizontalmente
	if (myPitch > 89.0f)
		myPitch = 89.0f;
	if (myPitch < -89.0f)
		myPitch = -89.0f;

	vec3 camF;
	camF.x = cos(radians(myYaw)) * cos(radians(myPitch));
	camF.y = sin(radians(myPitch));
	camF.z = sin(radians(myYaw)) * cos(radians(myPitch));
	cameraFront = normalize(camF);
}

void Camera::MouseScroll(GLFWwindow* window, double xScroll, double yScroll) {

	cout << myFov << endl;
	myFov = clamp((float)(myFov - yScroll), 1.f, 120.f);
}

mat4 Camera::LookAt() {

	vec3 target = cameraPos + cameraFront;
	vec3 direction = normalize(cameraPos - target);
	vec3 right = normalize(cross(vec3(0.f, 1.f, 0.f), direction));
	vec3 up = cross(direction, right);

	vec3 cameraF = direction;
	vec3 cameraR = right;
	vec3 cameraU = up;

	mat4 inversePos;
	inversePos[3][0] = -cameraPos.x;
	inversePos[3][1] = -cameraPos.y;
	inversePos[3][2] = -cameraPos.z;
	mat4 lookAt;
	lookAt[0][0] = cameraR.x;
	lookAt[1][0] = cameraR.y;
	lookAt[2][0] = cameraR.z;
	lookAt[0][1] = cameraU.x;
	lookAt[1][1] = cameraU.y;
	lookAt[2][1] = cameraU.z;
	lookAt[0][2] = cameraF.x;
	lookAt[1][2] = cameraF.y;
	lookAt[2][2] = cameraF.z;

	return lookAt*inversePos;
}

GLfloat Camera::GetFOV() {
	return myFov;
}