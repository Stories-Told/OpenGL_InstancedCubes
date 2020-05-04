#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"

using namespace std;

#define numVAOs 1
#define numVBOs 2

// Variables for the camera and cube location
float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float timeFactor;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// Variables for the perspective
GLuint mvLoc, projLoc, vLoc, mLoc, tfLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, tMat;

// 36 vertices, 12 triangles, makes 2x2x2 cube places at origin
void setupVertices()
{
	float vertexPositions[108] =
	{
		-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f , -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f , -1.0f, 1.0f, -1.0f
	};

	// VAO
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	// VBO
	glGenBuffers(numVBOs, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// Loades the cube vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

// Performs single used tasks: reading in shader code
// and building the rendering program, and loading cube vertices
// into the VBO, as well as positions the cube and camera
void init(GLFWwindow* window)
{
	// Program that reads in the vertex and fragment shade .glsl files
	// And creates the vertex and fragment shaders
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	// Initalize the camera and cubes position values
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 600.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;

	// Initalizes the cube
	setupVertices();
}

// Displays the window
void display(GLFWwindow* window, double currentTime)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	// Enables the shaders
	glUseProgram(renderingProgram);
	
	// Get the uniform variables location for the projection,
	// camera, and cube from the vertex shader
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
	mLoc = glGetUniformLocation(renderingProgram, "m_matrix");

	// Build perspective matrix, camera matrix, and cube matrix
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));


	// Copy perspective, camera, and cube to corresponding uniform variables
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));

	timeFactor = ((float)currentTime);
	tfLoc = glGetUniformLocation(renderingProgram, "tf");
	glUniform1f(tfLoc, (float)timeFactor);


	// Enables the VBO containing the cube
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Adjust OpenGL settings and draw model
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 10000);

}

int main(void)
{
	// Check to make sure glfw initilized
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Create the window
	GLFWwindow* window = glfwCreateWindow(1200, 800, "CGP_03", NULL, NULL);
	glfwMakeContextCurrent(window);
	// Check to make sure glew initilized
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1); // Enables v-sync

	// Initilize the window
	init(window);

	// While loop to keep the window open
	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window); // Paints the screen
		glfwPollEvents(); // Handles key events like key presses
	}

	// Close the window
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}