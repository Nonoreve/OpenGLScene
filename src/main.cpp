#include <iostream>
#include <stack>

//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "animation/AnimMoveAction.h"
#include "animation/AnimRotateAction.h"
#include "animation/Animation.h"

#include "buffer/complexvertexbuffer.h"
#include "buffer/indexbuffer.h"
#include "buffer/vertexbufferlayout.h"

#include "geometry/Cube.h"
#include "geometry/Square.h"

#include "rendering/Camera.h"
#include "rendering/renderer.h"
#include "rendering/RenderedObject.h"

#include "light.h"
#include "material.h"
#include "Shader.h"
#include "texture.h"
#include "vertexarray.h"

#define _2D 2
#define _3D 3
#define COL_CHANNELS 3 // components to defien one color (rgb = 3, rgba = 4, ...)
#define TEX_CHANNELS 2 // dimension of textures coordinates
#define VP_TRIANGLE 3 // Vertices per triangle

Shader* setupShaders(const char* vertexPath, const char* fragmentPath) {

	FILE* fragmentShader = fopen(fragmentPath, "r");
	FILE* vertexShader = fopen(vertexPath, "r");
	if(!fragmentShader || !vertexShader) {
		std::cerr << "Error opening shader files (probably not found)" << std::endl;
		//exit(1);
	}

	auto shader = Shader::loadFromFiles(vertexShader, fragmentShader);
	fclose(fragmentShader);
	fclose(vertexShader);
// 	shader = Shader::loadFromStrings(
// 		"#version 330 core\nlayout(location = 0) in vec4 position;\nvoid main(){gl_Position = position;}",
// 		"#version 330 core\nlayout(location = 0) out vec4 color;\nvoid main(){color = vec4(1.0, 0.0, 0.0, 1.0);}");

	if (shader == 0 || shader == nullptr) {
		std::cerr << "Shader null" << std::endl;
		//exit(EXIT_FAILURE);
	}
	return shader;
}

int main(int argc, char* argv[]) {
	////////////////////////////////////////
	//SDL2 / OpenGL Context initialization :
	////////////////////////////////////////

	//Initialize SDL2
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
		return 0;
	}

	//init SDL_image
	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		ERROR("Could not load SDL_image.\n");
		return EXIT_FAILURE;
	}

	//Create a Window
	SDL_Window* window = SDL_CreateWindow("OpenGL Scene",                           //Titre
	                                      SDL_WINDOWPOS_UNDEFINED,               //X Position
	                                      SDL_WINDOWPOS_UNDEFINED,               //Y Position
	                                      WIDTH, HEIGHT,                         //Resolution
	                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN); //Flags (OpenGL + Show)

	//Initialize OpenGL Version (default version 3.0)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//Initialize the OpenGL Context (where OpenGL resources (Graphics card resources) lives)
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//Tells GLEW to initialize the OpenGL function with this version
	glewExperimental = GL_TRUE;
	glewInit();
	SDL_GL_SetSwapInterval(1);



	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		//Start using OpenGL to draw something on screen
		GLCall(glViewport(0, 0, WIDTH, HEIGHT)); //Draw on ALL the screen

		//The OpenGL background color (RGBA, each component between 0.0f and 1.0f)
		GLCall(glClearColor(0.0, 0.0, 0.0, 1.0)); //Full Black

		GLCall(glEnable(GL_DEPTH_TEST)); //Active the depth test

		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_BLEND));

		Renderer renderer;
		RenderedObject root;
		Camera camera;

		glm::vec3 matColor(1.0f, 1.0f, 1.0f);
		glm::vec4 propert(0.5f, 0.5f, 0.5f, 50.0f);
		Material defaultMat(matColor, propert);

		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec3 lightPos(0.0f, 1.0f, 0.0f);
		Light sun = Light(lightPos, lightColor);

		auto defaultShader = setupShaders("resources/Shaders/Tex2.vert", "resources/Shaders/Tex2.frag");
		defaultShader->bindAttributes(2, "v_Position", "v_UV");


		//J'ai enleve car long de changer tous les shader en 140
		/*
		auto lightShader = setupShaders("resources/Shaders/lightTex.vert", "resources/Shaders/lightTex.frag");
		lightShader->bindAttributes(3, "v_Position", "v_UV", "v_Normal");
		*/
		

		// TODO color * texture in shader
		// shader->SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		Texture texture("resources/img/NonoreveLogo.png");
		Texture cubeTexture("resources/img/grass.png");
		//shader->SetUniform1i("u_Texture", 0);


// 		const unsigned int VERTICES = 4;
		const unsigned int TRIANGLES = 2;
		const unsigned int POS_DIM = _2D; // either 2D or 3D
		const unsigned int BUF_COMPONENTS = 3; // number of arrays
		const unsigned int sizeOfFloat = static_cast<unsigned int>(sizeof(float));

		// TODO include in geometry
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		//IndexBuffer ib(indices, TRIANGLES * VP_TRIANGLE);

		VertexArray squareVA;
		Geometry* square = new Square();
		SubData squarePosData = {square->getNbVertices()* POS_DIM * sizeOfFloat, square->getVertices()};
		SubData squareTexData = {square->getNbVertices()* TEX_CHANNELS * sizeOfFloat, square->getUVs()};
		SubData squareNormData = {square->getNbVertices() * 3 * sizeOfFloat, square->getNormals()};
		ComplexVertexBuffer squareVB(square->getNbVertices() * (POS_DIM + TEX_CHANNELS + 3) * sizeof(float), square->getNbVertices(), BUF_COMPONENTS, squarePosData, squareTexData, squareNormData);

		VertexBufferLayout squareLayout;
		squareLayout.Push(POS_DIM,GL_FLOAT);
		squareLayout.Push(TEX_CHANNELS, GL_FLOAT);
		squareLayout.Push(3, GL_FLOAT);
		squareVA.addBuffer(squareVB, squareLayout);
		defaultShader->Bind();
		RenderedObject texSquare(squareVA, square, defaultMat, texture, root, defaultShader);
		squareVB.Unbind();
		defaultShader->Unbind();

		/*
		VertexArray cubeVA;
		Geometry* cube = new Cube();
		const unsigned int vertices = cube->getNbVertices();
		SubData posData = {vertices* _3D * sizeOfFloat, cube->getVertices()};
		SubData texData = {vertices* TEX_CHANNELS * sizeOfFloat, cube->getUVs()};
		SubData normalsData = {vertices* _3D * sizeOfFloat, cube->getNormals()};
		ComplexVertexBuffer cubeVB(vertices * (_3D + _3D + TEX_CHANNELS) * sizeof(float), vertices, 3, posData, texData, normalsData);

		VertexBufferLayout cubeLayout;
		cubeLayout.Push(_3D, GL_FLOAT);
		cubeLayout.Push(TEX_CHANNELS, GL_FLOAT);
		cubeLayout.Push(_3D, GL_FLOAT);
		cubeVA.addBuffer(cubeVB, cubeLayout);
		lightShader->Bind();
		RenderedObject box(cubeVA, cube, defaultMat, cubeTexture, root, lightShader);
		cubeVB.Unbind();
		lightShader->Unbind();
		*/


		std::stack<glm::mat4> matrices;
		float currentTime = 0.0f;

		glm::vec3 scaling(5, 5, 5);

		texSquare.Move(glm::vec3(5, -5, -10));
		texSquare.SetScale(scaling);

		/*
		box.Move(glm::vec3(-5, 5, -10));
		box.SetScale(scaling);
		*/

		bool isOpened = true;
		//Main application loop
		while(isOpened) {
			//Time in ms telling us when this frame started. Useful for keeping a fix framerate
			uint32_t timeBegin = SDL_GetTicks();

			//Fetch the SDL events
			SDL_Event event;
			while(SDL_PollEvent(&event)) {
				switch(event.type) {
				case SDL_WINDOWEVENT:
					switch(event.window.event) {
					case SDL_WINDOWEVENT_CLOSE:
						isOpened = false;
						break;
					}
					break;
				default:
					camera.inputMove(event);
				}
			}
			
			camera.UpdateView();


			renderer.Clear();
			currentTime += TIME_PER_FRAME_MS;
			root.AfficherRecursif(matrices, currentTime, camera, sun);

			/*{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = camera.getProjectionM() * camera.getViewM() * model;
				//shader->SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
				shader->SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, Shader(*shader));
				glDrawArrays(GL_TRIANGLES, 0, square->getNbVertices());
			}*/


			//Display on screen (swap the buffer on screen and the buffer you are drawing on)
			SDL_GL_SwapWindow(window);

			//Time in ms telling us when this frame ended. Useful for keeping a fix framerate
			uint32_t timeEnd = SDL_GetTicks();

			//We want FRAMERATE FPS
			if(timeEnd - timeBegin < TIME_PER_FRAME_MS)
				SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
		}
	} // inner scope to call all destructors before SDL_GL_DeleteContext
	//Free everything
	if(context != NULL)
		SDL_GL_DeleteContext(context);
	if(window != NULL)
		SDL_DestroyWindow(window);

	return 0;
}
