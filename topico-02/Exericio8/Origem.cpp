/* Tópico 02 - código adaptado de https://github.com/fellowsheep/CG-2022-1
 *
 * Adaptado por Mateus Dittberner Weber
 * para a disciplina de Processamento Gráfico - Ciência da Computação - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 21/03/2022
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

int setupGeometry();

const GLuint WIDTH = 1024, HEIGHT = 768;

bool rotateX=false, rotateY=false, rotateZ=false;

int main()
{
	
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

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

	// matriz model
	glm::mat4 model = glm::mat4(1);
	model = glm::rotate(
		model, 
		(GLfloat)glfwGetTime(), 
		glm::vec3(1.0f, 0.0f, 0.0f)
	);
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	// matriz view (posição e orientação da cam)
	glm::mat4 view = glm::mat4(1);
	view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f)
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
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1);

		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
			
		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		}

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 14 * 3);
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
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}



}

int setupGeometry()
{
	GLfloat vertices[] = {
		//x    y     z     r    g    b
		// lado 1
		-0.5, -0.5, -0.5, 1.0, 0.0, 0.0,	// A
		-0.5, -0.5,  0.5, 1.0, 0.0, 0.0,	// B
		 0.5, -0.5, -0.5, 1.0, 0.0, 0.0,	// C

		-0.5, -0.5,  0.5, 1.0, 0.0, 0.0,	// D
		 0.5, -0.5,  0.5, 1.0, 0.0, 0.0,	// E
		 0.5, -0.5, -0.5, 1.0, 0.0, 0.0,	// F

		 // lado 2
		-0.5, 0.5, -0.5, 0.0, 1.0, 0.0,		// G
		-0.5, 0.5,  0.5, 0.0, 1.0, 0.0,		// H
		 0.5, 0.5, -0.5, 0.0, 1.0, 0.0,		// I
		  
		-0.5, 0.5,  0.5, 0.0, 1.0, 0.0,		// J
		 0.5, 0.5,  0.5, 0.0, 1.0, 0.0,		// K
		 0.5, 0.5, -0.5, 0.0, 1.0, 0.0,		// L
		 
		 // lado 3
		-0.5, -0.5, -0.5, 0.0, 0.0, 1.0,	// M
		 0.5, -0.5, -0.5, 0.0, 0.0, 1.0,	// N
		-0.5,  0.5, -0.5, 0.0, 0.0, 1.0,	// O

		 0.5, -0.5, -0.5, 0.0, 0.0, 1.0,	// P
		-0.5,  0.5, -0.5, 0.0, 0.0, 1.0,	// Q
		 0.5,  0.5, -0.5, 0.0, 0.0, 1.0,	// R

		 // lado 4
		-0.5, -0.5, 0.5, 1.0, 1.0, 0.0,		// S
		 0.5, -0.5, 0.5, 1.0, 1.0, 0.0,		// T
		-0.5,  0.5, 0.5, 1.0, 1.0, 0.0,		// U

		 0.5, -0.5, 0.5, 1.0, 1.0, 0.0,		// V
		-0.5,  0.5, 0.5, 1.0, 1.0, 0.0,		// W
		 0.5,  0.5, 0.5, 1.0, 1.0, 0.0,		// X

		// lado 5
		-0.5, -0.5, -0.5, 1.0, 0.0, 1.0,	// Y
		-0.5, -0.5,  0.5, 1.0, 0.0, 1.0,	// Z
		-0.5,  0.5, -0.5, 1.0, 0.0, 1.0,	// AA

		-0.5, -0.5,  0.5, 1.0, 0.0, 1.0,	// AB
		-0.5,  0.5, -0.5, 1.0, 0.0, 1.0,	// AC
		-0.5,  0.5,  0.5, 1.0, 0.0, 1.0,	// AD

		// lado 6
		0.5, -0.5, -0.5, 1.0, 0.0, 1.0,		// AF
		0.5, -0.5,  0.5, 1.0, 0.0, 1.0,		// AG
		0.5,  0.5, -0.5, 1.0, 0.0, 1.0,		// AH

		0.5, -0.5,  0.5, 1.0, 0.0, 1.0,		// AI
		0.5,  0.5, -0.5, 1.0, 0.0, 1.0,		// AJ
		0.5,  0.5,  0.5, 1.0, 0.0, 1.0,		// AK

		// chão
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

