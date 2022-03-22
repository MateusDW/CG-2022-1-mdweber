/* T�pico 02 - c�digo adaptado de https://github.com/fellowsheep/CG-2022-1
 *
 * Adaptado por Mateus Dittberner Weber
 * para a disciplina de Processamento Gr�fico - Ci�ncia da Computa��o - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 21/03/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

int setupGeometry();

const GLuint WIDTH = 1024, HEIGHT = 768;

// camera
glm::vec3 cameraPos, cameraFront, cameraUp;
float yaw = -90.0f, pitch = 0.0f;

// tempo
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// mouse
bool firstMouse = false;
float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;

int main()
{
	
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	const GLubyte* renderer = glGetString(GL_RENDERER); 
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader shader = Shader("../shaders/hello.vs", "../shaders/hello.fs");

	GLuint VAO = setupGeometry();

	shader.Use();

	// inicializa��o c�mera
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// matriz model
	glm::mat4 model = glm::mat4(1);
	model = glm::rotate(
		model, 
		(GLfloat)glfwGetTime(), 
		glm::vec3(1.0f, 0.0f, 0.0f)
	);
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	// matriz view (posi��o e orienta��o da cam)
	glm::mat4 view = glm::mat4(1);
	view = glm::lookAt(
		cameraPos,
		glm::vec3(0.0f, 0.0f, 0.0f), 
		cameraUp
	);
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	// matriz projection (profundidade)
	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(
		45.0f,
		(GLfloat) WIDTH / (GLfloat) HEIGHT,
		0.1f,
		100.0f
	);
	GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, FALSE, glm::value_ptr(projection));


	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 14 * 3); // 14 pontos com 3 coordenadas
		glDrawArrays(GL_POINTS, 0, 14 * 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	// deixando o movimento mais suave
	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;

	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

int setupGeometry()
{
	GLfloat vertices[] = {
		//x    y     z     r    g    b
		// frente (vermelho)
		-0.5, -0.5, 0.5, 1.0, 0.0, 0.0,
		 0.5, -0.5, 0.5, 1.0, 0.0, 0.0,
		-0.5,  0.5, 0.5, 1.0, 0.0, 0.0,

		 0.5, -0.5, 0.5, 1.0, 0.0, 0.0,
		-0.5,  0.5, 0.5, 1.0, 0.0, 0.0,
		 0.5,  0.5, 0.5, 1.0, 0.0, 0.0,

		 // tr�s (verde)
		-0.5, -0.5, -0.5, 0.0, 1.0, 0.0,
		 0.5, -0.5, -0.5, 0.0, 1.0, 0.0,
		-0.5,  0.5, -0.5, 0.0, 1.0, 0.0,

		 0.5, -0.5, -0.5, 0.0, 1.0, 0.0,
		-0.5,  0.5, -0.5, 0.0, 1.0, 0.0,
		 0.5,  0.5, -0.5, 0.0, 1.0, 0.0,

		 // esquerda (azul)
		-0.5, -0.5, -0.5, 0.0, 0.0, 1.0,
		-0.5, -0.5,  0.5, 0.0, 0.0, 1.0,
		-0.5,  0.5, -0.5, 0.0, 0.0, 1.0,

		-0.5, -0.5,  0.5, 0.0, 0.0, 1.0,
		-0.5,  0.5, -0.5, 0.0, 0.0, 1.0,
		-0.5,  0.5,  0.5, 0.0, 0.0, 1.0,

		// direita (amarelo)
		0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		0.5, -0.5,  0.5, 1.0, 1.0, 0.0,
		0.5,  0.5, -0.5, 1.0, 1.0, 0.0,

		0.5, -0.5,  0.5, 1.0, 1.0, 0.0,
		0.5,  0.5, -0.5, 1.0, 1.0, 0.0,
		0.5,  0.5,  0.5, 1.0, 1.0, 0.0,

		// cima (ciano)
		-0.5, 0.5, -0.5, 0.0, 1.0, 1.0,
		-0.5, 0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, 0.5, -0.5, 0.0, 1.0, 1.0,

		-0.5, 0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, 0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, 0.5, -0.5, 0.0, 1.0, 1.0,

		// baixo (rosa)
		-0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		-0.5, -0.5,  0.5, 1.0, 0.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0,

		-0.5, -0.5,  0.5, 1.0, 0.0, 1.0,
		 0.5, -0.5,  0.5, 1.0, 0.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0,

		// ch�o (cinza)
		-1, -0.5, -1, 0.5, 0.5, 0.5,
		-1, -0.5,  1, 0.5, 0.5, 0.5,
		 1, -0.5, -1, 0.5, 0.5, 0.5,

		-1, -0.5,  1, 0.5, 0.5, 0.5,
		 1, -0.5,  1, 0.5, 0.5, 0.5,
		 1, -0.5, -1, 0.5, 0.5, 0.5,
	};

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);



	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}

void processInput(GLFWwindow *window)
{
	// verifica callbacks de input
	glfwPollEvents();

	float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // mover para frente
	{
		cameraPos += cameraSpeed * cameraFront;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // mover para tr�s
	{
		cameraPos -= cameraSpeed * cameraFront;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // mover para esquerda
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // mover para direita
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

