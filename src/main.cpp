#include <iostream>

//SDL Libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//OpenGL Libraries
#include <GL/glew.h>
#include <GL/gl.h>

//GML libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "logger.h"

#include "renderer.h"

#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "vertexarray.h"
#include "vertexbufferlayout.h"

Shader* setupShaders(const char* vertexPath, const char* fragmentPath) {

	FILE* fragmentShader = fopen(fragmentPath, "r");
	FILE* vertexShader = fopen(vertexPath, "r");
	if (!fragmentShader || !vertexShader) {
		std::cerr << "Error opening shader files" << std::endl;
		exit(1);
	}

	auto shader = Shader::loadFromFiles(vertexShader, fragmentShader);
// 	shader = Shader::loadFromStrings(
// 		"#version 330 core\nlayout(location = 0) in vec4 position;\nvoid main(){gl_Position = position;}",
// 		"#version 330 core\nlayout(location = 0) out vec4 color;\nvoid main(){color = vec4(1.0, 0.0, 0.0, 1.0);}");
	if (shader == 0)
		exit(-1);
	return shader;
}

int main(int argc, char* argv[]) {
	////////////////////////////////////////
	//SDL2 / OpenGL Context initialization :
	////////////////////////////////////////

	//Initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		ERROR("The initialization of the SDL failed : %s\n", SDL_GetError());
		return 0;
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


		float positions[] = {
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.5f,  0.5f,

			-0.5f,  0.5f
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		
		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.addBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		auto shader = setupShaders("Shaders/color.vert", "Shaders/color.frag");
		shader->Bind();

		shader->SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		float r = 0.0f;
		float increment = 0.05f;

		shader->Unbind();
		vb.Unbind();
		ib.Unbind();
		
		Renderer renderer;


		bool isOpened = true;
		//Main application loop
		while (isOpened) {
			//Time in ms telling us when this frame started. Useful for keeping a fix framerate
			uint32_t timeBegin = SDL_GetTicks();

			//Fetch the SDL events
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
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
			
			
			renderer.Clear();

			shader->Bind();
			shader->SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

			renderer.Draw(va, ib, Shader(*shader));


			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;

			r += increment;


			//Display on screen (swap the buffer on screen and the buffer you are drawing on)
			SDL_GL_SwapWindow(window);

			//Time in ms telling us when this frame ended. Useful for keeping a fix framerate
			uint32_t timeEnd = SDL_GetTicks();

			//We want FRAMERATE FPS
			if (timeEnd - timeBegin < TIME_PER_FRAME_MS)
				SDL_Delay(TIME_PER_FRAME_MS - (timeEnd - timeBegin));
		}
	} // inner scope to call all destructors before SDL_GL_DeleteContext
	//Free everything
	if (context != NULL)
		SDL_GL_DeleteContext(context);
	if (window != NULL)
		SDL_DestroyWindow(window);

	return 0;
}
