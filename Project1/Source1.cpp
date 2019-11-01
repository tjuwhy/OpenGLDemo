#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <direct.h>

#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "LoadObj.h"
#include "Point.h"

using namespace std;


const double PI = 3.141592653589793238462643383279502884197169399;

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 810;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void load(char* fileName);
void getSphere();

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

Point getPoint(double u, double v);

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

int ustepNum = 100, vstepNum = 50;
vector<Point> points(2 + (vstepNum-1) * (ustepNum+1));
vector<int> indexes;
Point up = getPoint(0, 0);
Point down = getPoint(1, 1);

vector<float> coords(2 * (2 + (vstepNum - 1) * (ustepNum+1)));

// Camera
Camera camera(glm::vec3(0.0f, -5.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

//地球自转角度
float speedAngle = 10/3;
float currentAngle = 0;
//地球公转角度
float speedAngleEarth = 10;
float currentAngleEarth = 0;


//Light Position
glm::vec3 lightPos(0.8f, 1.0f, -0.2f);

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame



int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		printf("Failure");

		glfwTerminate();
		return -1;
	}

	// Set the required callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwMakeContextCurrent(window);

	

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Error");
		return -1;
	}
	
	glEnable(GL_DEPTH_TEST);

	Shader shader("vert.glsl", "frag.glsl");

	getSphere();
	
	ofstream fout("sphere.uniform.obj");
	bool ok = true;
	if (!fout)
	{
		cout << "File open failed." << endl;
		ok = false;
	}
	fout << "# vertices: " << points.size() << endl;
	fout << "# faces: " << indexes.size() / 3 << endl;
	vector<int> times(mesh.verts.size());
	float* vertices = new float[points.size() * 5];
	for (int i = 0; i < points.size() ; i++){
		vertices[5 * i] = points[i].x;
		vertices[5 * i + 1] = points[i].y;
		vertices[5 * i + 2] = points[i].z;
		if (ok)
		{
			fout << "v " << points[i].x << " " << points[i].y << " " << points[i].z << endl;
		}
		vertices[5 * i + 3] = coords[2*i];
		vertices[5 * i + 4] = coords[2*i+1];
		//cout << vertices[5 * i] << "  " << vertices[5 * i + 1] << " " << vertices[5 * i + 2] << endl;
	}
	unsigned int * indices = new unsigned int[indexes.size()];
	for (int i = 0; i < indexes.size(); i++)
	{
		indices[i] = indexes[i];
		if (ok){

			if (i % 3 == 0)
			{
				fout << "f " << indexes[i];
			}
			if (i % 3 == 1)
			{
				fout << indexes[i] << " ";
			}
			if (i % 3 == 2){
				fout << indexes[i] << endl;
			}
		}
	}
	fout.close();


	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size() * 5 + 10, vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexes.size(), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)( 3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);



	// load and create a texture 
	// -------------------------
	string names[5] = { "sun.jpg", "earth.jpg" };
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

	
	unsigned int textures[2] = {0};
	for (int i = 0; i < 2; i++)
	{
		glGenTextures(1, &textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load image, create texture and generate mipmaps
		// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
		unsigned char *data = stbi_load(string(names[i]).c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			cout << names[i] + "ok" << endl;

		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
	
	

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		shader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glUniform1i(glGetUniformLocation(shader.Program, "texture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glUniform1i(glGetUniformLocation(shader.Program, "texture2"), 1);

		glfwPollEvents();
		do_movement();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(shader.Program, "model");
		GLint viewLoc = glGetUniformLocation(shader.Program, "view");
		GLint projLoc = glGetUniformLocation(shader.Program, "projection");
		GLint indexLoc = glGetUniformLocation(shader.Program, "index");
		
		glm::mat4 model(1);
		//model = glm::rotate(model, glm::radians(currentAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		// Pass the matrices to the shader
		// Camera/View transformation
		glm::mat4 view(1);
		view = camera.GetViewMatrix();
		// Projection
		glm::mat4 projection(1);
		projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		glUniform1i(indexLoc, 0);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		
		glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0); // no need to it every time 

		model = glm::mat4(1);
		model = glm::rotate(model, glm::radians(currentAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-2.2f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, glm::radians(currentAngleEarth), glm::vec3(0.0f, 0.0f, 1.0f));

		glUniform1i(indexLoc, 1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------

		glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

void getSphere(){
	double ustep = 1 / (double)ustepNum, vstep = 1 / (double)vstepNum;
	points[0] = getPoint(0, 0);
	coords[0] = 0.5;
	coords[1] = 1;
	int c = 1;
	for (int i = 1; i < vstepNum; i++)
	{
		for (int j = 0; j <= ustepNum; j++)
		{ 
			points[c] = getPoint(ustep*j, vstep*i);
			coords[2 * c] = 1- ustep*j;
			coords[2 * c + 1 ] = vstep*i;
			c++;
		}
	}
	points[c] = getPoint(1, 1);
	coords[c++] = 0.5;
	coords[c] = 0;
	for (int i = 0; i <= ustepNum ; i++)//球体上第一层
	{
		indexes.push_back(0);//上顶点总是第一个点
		indexes.push_back(1 + i);
		indexes.push_back(1 + (i + 1) % (ustepNum+1));
		//cout << points[1 + i].x << " " << points[1 + i].y << " " << points[1 + i].z << endl;
		//cout << points[1 + (i + 1) % vstepNum].x << " " << points[1 + (i + 1) % vstepNum].y << " " << points[1 + (i + 1) % vstepNum].z << endl;
	}
	//int a = ustepNum+1;
	for (int i = 1; i < vstepNum - 1; i++){
		int start = 1 + (i - 1)*(ustepNum+1);
		for (int j = start; j < start + (1+ustepNum); j++){

			/*
			*       j
			*       |\
			*       | \
			*       |__\
			* j+ustepn j + (ustepnum + 1) % vstepnum
			*/
			indexes.push_back(j);
			indexes.push_back(j + (1+ustepNum));
			indexes.push_back(start + (1+ustepNum) + (j + 1 - start) % (1+ustepNum));
			/*
			* 
			*         j ___ （j + 1）% ustepnum
			*           \  |
			*		     \ |
			*		      \|
			*     j + (ustepnum + 1) % vstepnum
			*/

			indexes.push_back(j);
			indexes.push_back(start + (j + 1 - start) % (1+ustepNum));
			indexes.push_back(start + ustepNum + 1 + (j + 1 - start) %(1+ ustepNum));
			}
	}
	int last = 1 + (ustepNum+1) * (vstepNum - 1);
	int start = 1 + (ustepNum+1) * (vstepNum - 2);
	for (int i = 1 + (ustepNum+1) * (vstepNum - 2); i < 1 + ustepNum * (vstepNum - 1); i++)//球体上最后一层
	{
		indexes.push_back(i); 
		indexes.push_back(last); //逆时针排列
		indexes.push_back(start + ((1 + i) - start) % (1+ustepNum));
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 5.0f * deltaTime;
	currentAngle = (currentAngle + speedAngle* deltaTime);
	currentAngleEarth += 10 * deltaTime;
	if (currentAngle > 360.0 + 1e-10)
	{
		currentAngle -= 360;
	}
	if (currentAngleEarth > 360.0 + 1E-10)
	{
		currentAngleEarth -= 360;
	}
	if (keys[GLFW_KEY_W])
		if (speedAngle < 0 / 3)
		{
			speedAngle += 0.01;
		}
	if (keys[GLFW_KEY_S])
		if (speedAngle > 5 / 3)
		{
			speedAngle -= 0.01;
		}
	if (keys[GLFW_KEY_A]){
		if (speedAngleEarth < 40){
			speedAngleEarth += 0.03;
		}
	}
	if (keys[GLFW_KEY_D]){
		if (speedAngleEarth > 5){
			speedAngleEarth -= 0.03;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	//if (firstMouse)
	//{
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

	//GLfloat xoffset = xpos - lastX;
	//GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
	//lastX = xpos;
	//lastY = ypos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

Point getPoint(double u, double v){
	double x = sin(PI * v)*cos(2 * PI * u);
	double y = sin(PI * v)*sin(2 * PI * u);
	double z = cos(PI * v);
	return Point(x, y, z);
}