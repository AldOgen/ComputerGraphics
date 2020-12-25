#pragma once
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


void InitializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}


void GLADLoader() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "ERROR::GLAD::FAILED_TO_INITIALIZE_GLAD" << std::endl;
		exit(-1);
	}
}