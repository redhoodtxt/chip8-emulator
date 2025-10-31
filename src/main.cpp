#include <iostream>
#include <cstdio>
#include <algorithm>
#include <chrono>
#include "chip8.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaders.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

bool keys[512] = { false };
int scancodes[16];

// vertex/element data for a single quad (pixel)
float vertices[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

// struct to hold buffer handles
struct Buffers {
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};

Buffers genBuffers() {
	Buffers buf;
	glGenVertexArrays(1, &buf.VAO);
	glGenBuffers(1, &buf.VBO);
	glGenBuffers(1, &buf.EBO);

	glBindVertexArray(buf.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buf.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	return buf;
}

static unsigned int createShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type); // create a shader object
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL); // add the shader source code to the shader object created

	return id;
}

static int compileShader(const std::string& vertex_shader, const std::string& fragment_shader) { // TODO: change parameter type
	unsigned int program = glCreateProgram(); // sticking to cpp type instead of GLuint

	unsigned int vshader = createShader(GL_VERTEX_SHADER, vertex_shader); // create vertex shader
	glCompileShader(vshader); // compile fragment shader

	int isCompiled; // flag to check if compiled 
	char info_log[512]; // array to store the info log 

	// check if vertex shader compilation successful
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &isCompiled); // get the compile status 
	if (!isCompiled) {
		glGetShaderInfoLog(vshader, 512, NULL, info_log);
		std::cout << "ERROR: vertex shader compilation failed\n" << info_log << std::endl;
	}

	unsigned int fshader = createShader(GL_FRAGMENT_SHADER, fragment_shader); // create fragment shader
	glCompileShader(fshader); // compile fragment shader

	// check if fragment shader compilation successful 
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &isCompiled);
	if (!isCompiled) {
		glGetShaderInfoLog(fshader, 512, NULL, info_log);
		std::cout << "ERROR: fragment shader compilation failed\n" << info_log << std::endl;
	}

	// attach the 2 shaders to the program
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program); // link the program


	glValidateProgram(program);

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	return program;

}

void initGLFW() {
	// initialize GLFW
	glfwInit();
	// indicate OpenGL version 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 3.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 0.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // enable legacy functions like glBegin()
}

GLFWwindow* createWindow(int width, int height, const char* title) {
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	return window;
}

bool initGLAD() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void configureOpenGL(GLFWwindow* window) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);  // ¡û Use actual size!

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
}

void renderDisplay(GLFWwindow* window, Chip8& chip8, Shader& shader, unsigned int VAO) {
	glClear(GL_COLOR_BUFFER_BIT);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	float pixelWidth = (float)width / 64.0f;
	float pixelHeight = (float)height / 32.0f;

	// maintain aspect ratio ¡ª use min to keep full screen visible
	float pixelSize = std::min(pixelWidth, pixelHeight);
	// recalc offsets so screen is centered
	float displayWidth = 64.0f * pixelSize;
	float displayHeight = 32.0f * pixelSize;
	float offsetX = (width - displayWidth) / 2.0f;
	float offsetY = (height - displayHeight) / 2.0f;

	glm::mat4 proj = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
	shader.use();
	shader.setMat4("projection", proj);
	shader.setVec3("color", glm::vec3(1.0f));

	glBindVertexArray(VAO);

	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 64; x++) {
			if (chip8.display[x][y]) {
				float px = offsetX + x * pixelSize;
				float py = offsetY + y * pixelSize;

				shader.setVec2("offset", glm::vec2(px, py));
				shader.setVec2("scale", glm::vec2(pixelSize, pixelSize));
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}
		}
	}

	glBindVertexArray(0);
}

// all you weirdos using different keyboard layouts making my life harder so i use scancodes instead of physical key logic
void initScancodes(int scancodes[]) {
	/*
	©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¤©´
	©¦ Chip-8 Key ©¦ PC Key ©¦
	©À©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©à©¤©¤©¤©¤©¤©¤©¤©¤©È
	©¦ 1          ©¦ 1      ©¦
	©¦ 2          ©¦ 2      ©¦
	©¦ 3          ©¦ 3      ©¦
	©¦ C          ©¦ 4      ©¦
	©¦ 4          ©¦ Q      ©¦
	©¦ 5          ©¦ W      ©¦
	©¦ 6          ©¦ E      ©¦
	©¦ D          ©¦ R      ©¦
	©¦ 7          ©¦ A      ©¦
	©¦ 8          ©¦ S      ©¦
	©¦ 9          ©¦ D      ©¦
	©¦ E          ©¦ F      ©¦
	©¦ A          ©¦ Z      ©¦
	©¦ 0          ©¦ X      ©¦
	©¦ B          ©¦ C      ©¦
	©¦ F          ©¦ V      ©¦
	©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ø©¤©¤©¤©¤©¤©¤©¤©¤©¼
	*/
	scancodes[0x1] = glfwGetKeyScancode(GLFW_KEY_1);
	scancodes[0x2] = glfwGetKeyScancode(GLFW_KEY_2);
	scancodes[0x3] = glfwGetKeyScancode(GLFW_KEY_3);
	scancodes[0xc] = glfwGetKeyScancode(GLFW_KEY_4);

	scancodes[0x4] = glfwGetKeyScancode(GLFW_KEY_Q);
	scancodes[0x5] = glfwGetKeyScancode(GLFW_KEY_W);
	scancodes[0x6] = glfwGetKeyScancode(GLFW_KEY_E);
	scancodes[0xd] = glfwGetKeyScancode(GLFW_KEY_R);

	scancodes[0x7] = glfwGetKeyScancode(GLFW_KEY_A);
	scancodes[0x8] = glfwGetKeyScancode(GLFW_KEY_S);
	scancodes[0x9] = glfwGetKeyScancode(GLFW_KEY_D);
	scancodes[0xe] = glfwGetKeyScancode(GLFW_KEY_F);

	scancodes[0xa] = glfwGetKeyScancode(GLFW_KEY_Z);
	scancodes[0x0] = glfwGetKeyScancode(GLFW_KEY_X);
	scancodes[0xb] = glfwGetKeyScancode(GLFW_KEY_C);
	scancodes[0xf] = glfwGetKeyScancode(GLFW_KEY_V);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		keys[scancode] = true;
	}
	else if (action == GLFW_RELEASE) {
		keys[scancode] = false;
	}
}

void processInput(GLFWwindow* window, Chip8& chip8, int scancodes[]) {
	for (int i = 0; i < 16; i++) {
		chip8.keypad[i] = keys[scancodes[i]];
	}

	// escape key pressed to exit program
	if (keys[glfwGetKeyScancode(GLFW_KEY_ESCAPE)]) {
		glfwSetWindowShouldClose(window, true);
	}
}

void renderLoop(GLFWwindow* window, Chip8& chip8, Shader& shader, unsigned int VAO) {
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Debug: print keypad state
		static int frameCount = 0;
		if (frameCount++ % 60 == 0) {  // Print every 60 frames
			printf("Keypad state: ");
			for (int i = 0; i < 16; i++) {
				if (chip8.keypad[i]) printf("%X ", i);
			}
			printf("\n");
		}

		processInput(window, chip8, scancodes);

		for (int i = 0; i < 50; i++) {
			chip8.emulateCycle();
		}

		// update timers at 60Hz
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		if (elapsed >= 16) {  // ~60Hz
			if (chip8.delay_timer > 0) chip8.delay_timer--;
			if (chip8.sound_timer > 0) chip8.sound_timer--;
			lastTime = currentTime;
		}

		renderDisplay(window, chip8, shader, VAO);
		glfwSwapBuffers(window);
	}
}

void cleanup(GLFWwindow* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main() {
	// create chip8 object
	Chip8 chip8;
	chip8.init();
	chip8.loadGame(); // load rom

	// GLFW + OpenGL init
	initGLFW();
	GLFWwindow* window = createWindow(1920, 1080, "chip-8");
	if (!window) return -1;
	if (!initGLAD()) return -1;

	configureOpenGL(window);

	initScancodes(scancodes);
	
	glfwSetKeyCallback(window, keyCallback); // regsiter callback
	// VAO/VBO/EBO
	Buffers buffers = genBuffers();

	// shader program
	Shader shader;
	auto sources = shader.loadShaderSource("C:/Users/Admin/Documents/gengz/projects/chip8-emulator/res/shaders/vs.glsl", "C:/Users/Admin/Documents/gengz/projects/chip8-emulator/res/shaders/fs.glsl");
	unsigned int shaderProgram = compileShader(sources.first, sources.second);
	if (!shaderProgram) {
		std::cerr << "Failed to compile shaders\n";
		return -1;
	}
	shader.ID = shaderProgram;
	// for now, no render function. We just set up buffers and shaders.
	renderLoop(window, chip8, shader, buffers.VAO);

	glDeleteVertexArrays(1, &buffers.VAO);
	glDeleteBuffers(1, &buffers.VBO);
	glDeleteBuffers(1, &buffers.EBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}