#include <iostream>
#include <cstdio>
#include "chip8.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void initGLFW(){
	// initialize GLFW
	glfwInit();
	// indicate OpenGL version 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 3.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 0.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // use core profile - only modern functions
}

GLFWwindow* createWindow(int width, int height, const char* title){
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}	
	glfwMakeContextCurrent(window);
	return window;
}

bool initGLAD(){
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cout << "Failed to initialize GLAD" << std::endl;
    return false;
	}
	return true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

void configureOpenGL(){
	glViewport(0, 0, 2560, 1440);
	glfwSetFramebufferSizeCallback(glfwGetCurrentContext(), framebuffer_size_callback); // allow for resizing
	glClearColor(0.00f, 0.00f, 0.00f, 0.00f); // black bg
	glClear(GL_COLOR_BUFFER_BIT); // clear back buffer
	glfwSwapBuffers(glfwGetCurrentContext()); // swap the back buffer with the front
}

void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // escape key pressed to exit program
        glfwSetWindowShouldClose(window, true);
}

void renderLoop(GLFWwindow* window){
	while(!glfwWindowShouldClose(window)){
		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void cleanup(GLFWwindow* window){
	glfwDestroyWindow(window);
	glfwTerminate();
}

// create chip8 object
Chip8 mainChip8;
 
int main(int argc, char **argv){
    initGLFW();
	GLFWwindow* window = createWindow(2560, 1440, "chip-8" );
	initGLAD();
	configureOpenGL();
	renderLoop(window);
	cleanup(window);

    return 0;
}
    