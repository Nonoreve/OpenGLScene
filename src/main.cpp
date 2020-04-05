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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "animation/AnimMoveAction.h"
#include "animation/AnimRotateAction.h"
#include "animation/Animation.h"

#include "buffer/complexvertexbuffer.h"
#include "buffer/indexbuffer.h"
#include "buffer/vertexbufferlayout.h"

#include "geometry/Cube.h"

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
		std::cerr << "Error opening shader files" << std::endl;
		exit(1);
	}

	auto shader = Shader::loadFromFiles(vertexShader, fragmentShader);
	fclose(fragmentShader);
	fclose(vertexShader);
// 	shader = Shader::loadFromStrings(
// 		"#version 330 core\nlayout(location = 0) in vec4 position;\nvoid main(){gl_Position = position;}",
// 		"#version 330 core\nlayout(location = 0) out vec4 color;\nvoid main(){color = vec4(1.0, 0.0, 0.0, 1.0);}");
	if(shader == 0)
		exit(EXIT_FAILURE);
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

		/*const unsigned int VERTICES = 4;
		const unsigned int TRIANGLES = 2;
		const unsigned int POS_DIM = _2D; // either 2D or 3D
		const unsigned int BUF_COMPONENTS = 2; // number of arrays

		float positions[] = {
			-50.0f, -50.0f,
			50.0f, -50.0f,
			50.0f,  50.0f,
			-50.0f,  50.0f
		};

		float texturesPositions[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		VertexArray va;
		SubData posData = {VERTICES* POS_DIM * sizeof(float), positions};
		SubData texData = {VERTICES* POS_DIM * sizeof(float), texturesPositions};
		ComplexVertexBuffer vb(VERTICES * (POS_DIM + TEX_CHANNELS) * sizeof(float), VERTICES, BUF_COMPONENTS, posData, texData);

		VertexBufferLayout layout;
		layout.Push<float>(POS_DIM);
		layout.Push<float>(TEX_CHANNELS);
		va.addBuffer(vb, layout);

		IndexBuffer ib(indices, TRIANGLES * VP_TRIANGLE);

		// TODO included in camera
		glm::mat4 proj = glm::ortho(0.0f, (float)(WIDTH), 0.0f, (float)(HEIGHT), -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));*/

		auto shader = setupShaders("Shaders/Tex.vert", "Shaders/Tex.frag");
		shader->bindAttributes(2, "v_Position", "v_UV");
		
		auto lightShader = setupShaders("Shaders/lightTex.vert", "Shaders/lightTex.frag");
		lightShader->bindAttributes(3, "v_Position", "v_UV", "v_Normal");
		
		shader->Bind();
		// TODO color * texture in shader
		// shader->SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		// TODO make portable
		Texture texture(RESOURCE_FILE("NonoreveLogo.png"));
		texture.Bind();
		//shader->SetUniform1i("u_Texture", 0);

		/*glm::vec3 translationA(420, 69, 0);
		glm::vec3 translationB(69, 420, 0);

		float r = 0.0f;
		float increment = 0.05f;

		shader->Unbind();
		vb.Unbind();
		ib.Unbind();

		Renderer renderer;*/
		
		glm::vec3 matColor(1.0f, 1.0f, 1.0f);
		glm::vec4 propert(0.5f, 0.5f, 0.5f, 50.0f);
		
		Material defaultMat(matColor, propert);
		
		VertexArray va;
		RenderedObject root;
		Geometry* cube = new Cube();
		const unsigned int vertices = cube->getNbVertices();
		
		SubData posData = {vertices * 3 * static_cast<unsigned int>(sizeof(float)), cube->getVertices()};
		SubData normalsData = {vertices * 3 * sizeof(float), cube->getNormals()};
		SubData texData = {vertices * 2 * sizeof(float), cube->getUVs()};
		ComplexVertexBuffer vb(vertices * (3 + 3 + 2) * sizeof(float), vertices, 3, posData, normalsData, texData);
		
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(TEX_CHANNELS);
		va.addBuffer(vb, layout);
		RenderedObject box(cube, &defaultMat, root, shader);
		
		std::stack<glm::mat4> matrices;
		float currentTime = 0.0f;


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
					default:
						break;
					}
					break;
					//We can add more event, like listening for the keyboard or the mouse. See SDL_Event documentation for more details
				}
			}


			/*renderer.Clear();

			shader->Bind();
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				//shader->SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
				shader->SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, Shader(*shader));
			}

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				//shader->SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
				shader->SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, Shader(*shader));
			}

			if(r > 1.0f)
				increment = -0.05f;
			else if(r < 0.0f)
				increment = 0.05f;

			r += increment;*/
			
			
			//const unsigned int TEXTURE_SLOT = 0;
			//m_Texture.Bind(TEXTURE_SLOT);
			GLCall(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
			va.Bind();
			currentTime += TIME_PER_FRAME_MS;
			root.AfficherRecursif(matrices, currentTime);


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
