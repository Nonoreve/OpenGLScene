#include <iostream>
#include <stack>
#include <cstdarg>

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
#include "geometry/ObjMesh.h"


#include "rendering/Camera.h"
#include "rendering/renderer.h"
#include "rendering/RenderedObject.h"

#include "light.h"
#include "material.h"
#include "Shader.h"
#include "texture.h"
#include "vertexarray.h"



Shader* setupShaders(const char* vertexPath, const char* fragmentPath, unsigned int count, ...) {
	va_list args;
	va_start(args, count);
	FILE* fragmentShader = fopen(fragmentPath, "r");
	FILE* vertexShader = fopen(vertexPath, "r");
	if (!fragmentShader || !vertexShader) {
		std::cerr << "[File Error] can't open shader" << std::endl;
		//exit(1);
	}

	auto shader = Shader::loadFromFiles(vertexShader, fragmentShader, count, args);
	va_end(args);
	fclose(fragmentShader);
	fclose(vertexShader);
	// 	shader = Shader::loadFromStrings(
	// 		"#version 330 core\nlayout(location = 0) in vec4 position;\nvoid main(){gl_Position = position;}",
	// 		"#version 330 core\nlayout(location = 0) out vec4 color;\nvoid main(){color = vec4(1.0, 0.0, 0.0, 1.0);}");

	if (shader == 0 || shader == nullptr) {
		std::cerr << "[Shader Error] NULL" << std::endl;
		//exit(EXIT_FAILURE);
	}
	return shader;
}

void drawPortail(RenderedObject &intPortail, RenderedObject &portail, float &currentTime, Camera &camera, Light &sun) {

	std::stack<glm::mat4> matrices;
	{
		//TODO Comme les propriétées du portail n'est pas la meme mais ils héritent du meme objet il faudrait faire la multiplication a la main ici

		glDisable(GL_STENCIL_TEST);

		portail.AfficherRecursif(matrices, currentTime, camera, sun);
	}
	{
		matrices = std::stack<glm::mat4>();

		glEnable(GL_STENCIL_TEST);

		/*
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		*/

		// Draw floor
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glStencilMask(0xFF); // Write to stencil buffer

		glDepthMask(GL_FALSE); // Don't write to depth buffer

		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

		intPortail.AfficherRecursif(matrices, currentTime, camera, sun);

		glDepthMask(GL_TRUE); // Don't write to depth buffer

	}
}

void drawCurrentWorld(RenderedObject &r, float &currentTime, Camera &camera, Light &sun) {
	std::stack<glm::mat4> matrices;

	glDisable(GL_STENCIL_TEST);

	r.AfficherRecursif(matrices, currentTime, camera, sun);

}


void drawOtherWorld(RenderedObject &r, float &currentTime, Camera &camera, Light &sun) {


	std::stack<glm::mat4> matrices;


	// Draw
	glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1


	glStencilMask(0x00); // Don't write anything to stencil buffer

	glEnable(GL_DEPTH_TEST);//Attention si il y a une erreur il faut penser a le changer


							//Attention peut mal fonctionner avec le fog

	glDepthMask(GL_TRUE); // Write to depth buffer

	r.AfficherRecursif(matrices, currentTime, camera, sun);

}


void ActiverWireframe() {

	glCullFace(GL_FRONT);
	glColor3f(0, 0, 0);
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(1, 1);

}

void DesactiverWireframe() {
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);
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

	//init SDL_image
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		ERROR("Could not load SDL_image.\n");
		return EXIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
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
		GLCall(glClearColor(0.3, 0.3, 0.3, 1.0)); //Gray

		GLCall(glEnable(GL_DEPTH_TEST)); //Active the depth test

		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_BLEND));


		int stencilBits = -1;
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilBits);
		std::cout << "stencil bit size (cense etre a 8) : " << stencilBits << std::endl;

		bool isWorldIle = true;
		bool isWireframe = false;

		Renderer renderer;
		RenderedObject root;
		Camera camera;

		glm::vec4 matColor(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec4 propert(0.5f, 0.5f, 0.5f, 50.0f);
		Material defaultMat(matColor, propert);

		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec4 lightPos(0.0f, 1.0f, 0.0f, 1.0f);
		Light sun = Light(lightPos, lightColor);

		auto defaultShader = setupShaders("resources/Shaders/Tex.vert", "resources/Shaders/Tex.frag", 2, "v_Position", "v_UV");

		auto lightShader = setupShaders("resources/Shaders/lightTex.vert", "resources/Shaders/lightTex.frag", 3, "v_Position", "v_UV", "v_Normal");


		Texture emptyTexture("resources/img/.png");
		Texture skyboxTexture("resources/img/skybox.png");
		Texture ileTexture("resources/img/ile.png");
		Texture papillonTexture("resources/img/papillon.png");
		Texture fondTexture("resources/img/fond.png");
		Texture portailTexture("resources/img/portail.png");

		// TODO include in geometry
		// 		const unsigned int VERTICES = 4;
		// 		const unsigned int TRIANGLES = 2;
		// 		unsigned int indices[] = {
		// 			0, 1, 2,
		// 			2, 3, 0
		// 		};
		// 		IndexBuffer ib(indices, TRIANGLES * VP_TRIANGLE);

		/*
		VertexArray squareVA;
		Geometry* square = new Square();
		ComplexVertexBuffer squareVB = square->bufferFactory();
		squareVA.addBuffer(squareVB, square->bufferLayoutFactory());
		defaultShader->Bind();
		RenderedObject texSquare(squareVA, square, defaultMat, texture, root, defaultShader);
		squareVB.Unbind();
		defaultShader->Unbind();

		VertexArray cubeVA;
		Geometry* cube = new Cube();
		ComplexVertexBuffer cubeVB = cube->bufferFactory();
		cubeVA.addBuffer(cubeVB, cube->bufferLayoutFactory());
		lightShader->Bind();
		RenderedObject box(cubeVA, cube, defaultMat, cubeTexture, root, lightShader);
		cubeVB.Unbind();
		lightShader->Unbind();

		*/
		//fish

		/*
		VertexArray fishVA;
		Geometry* fish = new ObjMesh("resources/Obj/fish.obj");
		ComplexVertexBuffer fishVB = fish->bufferFactory();
		fishVA.addBuffer(fishVB, fish->bufferLayoutFactory());
		defaultShader->Bind();
		RenderedObject fishR(fishVA, fish, defaultMat, fishTexture, root, defaultShader);
		fishVB.Unbind();
		defaultShader->Unbind();
		*/

		defaultShader->Bind();


		RenderedObject PARENT_PORTAIL(root);
		VertexArray portailVA;
		Geometry* portailG = new ObjMesh("resources/Obj/portail.obj");
		ComplexVertexBuffer portailVB = portailG->bufferFactory();
		portailVA.addBuffer(portailVB, portailG->bufferLayoutFactory());
		RenderedObject Portail(portailVA, portailG, defaultMat, portailTexture, PARENT_PORTAIL, defaultShader);

		VertexArray intPortailVA;
		Geometry* intPortailG = new ObjMesh("resources/Obj/intPortail.obj");
		ComplexVertexBuffer intPortailVB = intPortailG->bufferFactory();
		intPortailVA.addBuffer(intPortailVB, intPortailG->bufferLayoutFactory());
		RenderedObject IntPortail(intPortailVA, intPortailG, defaultMat, emptyTexture, PARENT_PORTAIL, defaultShader);

		IntPortail.Rotate(90, glm::vec3(1, 0, 0));
		Portail.Rotate(90, glm::vec3(1, 0, 0));


		RenderedObject PARENT_SOUS_MARIN(root);
		VertexArray fondVA;
		Geometry* fondG = new ObjMesh("resources/Obj/fond.obj");
		ComplexVertexBuffer fondVB = fondG->bufferFactory();
		fondVA.addBuffer(fondVB, fondG->bufferLayoutFactory());
		RenderedObject Fond(fondVA, fondG, defaultMat, fondTexture, PARENT_SOUS_MARIN, defaultShader);
		{
			Fond.SetScale(glm::vec3(0.5f, 0.3f, 0.5f));
			Fond.Move(glm::vec3(-15.0f, 4.0f, -10.0f));
		}



		RenderedObject PARENT_ILE(root);
		RenderedObject ParentPapillon(PARENT_ILE); {
			ParentPapillon.Move(glm::vec3(0.0f, 2.0f, 0.0f));
		
		}


		//We can use the same AnimRotateAction for each papillon
		VertexArray papillonVA;
		Geometry* papillonG = new ObjMesh("resources/Obj/papillon.obj");
		ComplexVertexBuffer papillonVB = papillonG->bufferFactory();
		papillonVA.addBuffer(papillonVB, papillonG->bufferLayoutFactory());
		RenderedObject AileG(papillonVA, papillonG, defaultMat, papillonTexture, ParentPapillon, defaultShader);
		{
			AileG.SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
			AileG.AddAnimation(new Animation(0.0f, 1000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
			AileG.AddAnimation(new Animation(1000.0f, 2000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
		}
		RenderedObject AileD(papillonVA, papillonG, defaultMat, papillonTexture, ParentPapillon, defaultShader);
		{
			AileD.SetScale(glm::vec3(-0.1f, 0.1f, 0.1f));
			AileD.AddAnimation(new Animation(0.0f, 1000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
			AileD.AddAnimation(new Animation(1000.0f, 2000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
		}
		papillonVB.Unbind();

		VertexArray ileVA;
		Geometry* ileG = new ObjMesh("resources/Obj/ile.obj");
		ComplexVertexBuffer ileVB = ileG->bufferFactory();
		ileVA.addBuffer(ileVB, ileG->bufferLayoutFactory());
		RenderedObject Ile(ileVA, ileG, defaultMat, ileTexture, PARENT_ILE, defaultShader);
		{
			Ile.Move(glm::vec3(0, -3, 0));
			Ile.SetScale(glm::vec3(0.6f, 0.6f, 0.6f));
		}
		ileVB.Unbind();

		VertexArray troncVA;
		Geometry* troncG = new ObjMesh("resources/Obj/tronc.obj");
		ComplexVertexBuffer troncVB = troncG->bufferFactory();
		troncVA.addBuffer(troncVB, troncG->bufferLayoutFactory());
		RenderedObject Tronc(troncVA, troncG, defaultMat, emptyTexture, Ile, defaultShader); 
		{
			Tronc.Move(glm::vec3(-3.0f, 0.0f, 3.0f));
		
		}
		troncVB.Unbind();


		VertexArray feuilleVA;
		Geometry* feuilleG = new ObjMesh("resources/Obj/feuille.obj");
		ComplexVertexBuffer feuilleVB = feuilleG->bufferFactory();
		feuilleVA.addBuffer(feuilleVB, feuilleG->bufferLayoutFactory());
		RenderedObject Feuille(feuilleVA, feuilleG, defaultMat, emptyTexture, Tronc, defaultShader);
		{
			Feuille.Move(glm::vec3(-5.5f, 12.7f, -2.4f));
			Feuille.Rotate(-30.0f,glm::vec3( 1.0f, 0.0f, -1.0f));

		}
		RenderedObject Feuille2(feuilleVA, feuilleG, defaultMat, emptyTexture, Tronc, defaultShader);
		{
			Feuille2.Move(glm::vec3(-0.8f, 13.0f, 0.0f));
			Feuille2.Rotate(110.0f,glm::vec3( 0.0f, 1.0f, 0.0f));
			Feuille2.Rotate(-30.0f,glm::vec3( 1.0f, 0.0f, -0.6f));
		}
		RenderedObject Feuille3(feuilleVA, feuilleG, defaultMat, emptyTexture, Tronc, defaultShader);
		{
			Feuille3.Move(glm::vec3(1.0f, 13.7f, -3.5f));
			Feuille3.Rotate(-30.0f, glm::vec3(1.0f, 0.0f, -1.0f));
			Feuille3.Rotate(220.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		feuilleVB.Unbind();

		VertexArray rocherVA;
		Geometry* rocherG = new ObjMesh("resources/Obj/rocher.obj");
		ComplexVertexBuffer rocherVB = rocherG->bufferFactory();
		rocherVA.addBuffer(rocherVB, rocherG->bufferLayoutFactory());
		RenderedObject Rocher(rocherVA, rocherG, defaultMat, emptyTexture, Ile, defaultShader);
		{
			Rocher.Move(glm::vec3(3.5f, -0.25f, 0));
		}
		rocherVB.Unbind();

		//+ Baleine

		VertexArray skyboxVA;
		Geometry* skyboxG = new ObjMesh("resources/Obj/skybox.obj");
		ComplexVertexBuffer skyboxVB = skyboxG->bufferFactory();
		skyboxVA.addBuffer(skyboxVB, skyboxG->bufferLayoutFactory());
		RenderedObject Skybox(skyboxVA, skyboxG, defaultMat, skyboxTexture, PARENT_ILE, defaultShader);
		{
			Skybox.Rotate(180.0f,glm::vec3( 0.0f, 1.0f, 0.0f));
			Skybox.SetScale(glm::vec3(40.0f, 40.0f, 40.0f));
		}

		


		std::stack<glm::mat4> matrices;
		float currentTime = 0.0f;

		glm::vec3 scaling(5, 5, 5);


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
					}
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_t:
						if (isWireframe)
							DesactiverWireframe();
						else
							ActiverWireframe();
						isWireframe = !isWireframe;
						break;
					}
				default:
					camera.inputMove(event);
				}
			}

			camera.UpdateView();


			renderer.Clear();
			currentTime += TIME_PER_FRAME_MS;

			drawCurrentWorld(PARENT_ILE, currentTime, camera, sun);
			drawPortail(IntPortail, Portail, currentTime, camera, sun);
			drawOtherWorld(PARENT_SOUS_MARIN, currentTime, camera, sun);

			/*
			PARENT_ILE.AfficherRecursif(matrices, currentTime, camera, sun);
			PARENT_PORTAIL.AfficherRecursif(matrices, currentTime, camera, sun);
			PARENT_SOUS_MARIN.AfficherRecursif(matrices, currentTime, camera, sun);
			*/

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