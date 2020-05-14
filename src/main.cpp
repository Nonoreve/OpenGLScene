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
	if(!fragmentShader) {
		std::cerr << "[File Error] can't open shader " << fragmentPath << std::endl;
		//exit(1);
	}
	if(!vertexShader) {
		std::cerr << "[File Error] can't open shader " << vertexPath << std::endl;
		//exit(1);
	}

	auto shader = Shader::loadFromFiles(vertexShader, fragmentShader, count, args);
	va_end(args);
	fclose(fragmentShader);
	fclose(vertexShader);
	// 	shader = Shader::loadFromStrings(
	// 		"#version 330 core\nlayout(location = 0) in vec4 position;\nvoid main(){gl_Position = position;}",
	// 		"#version 330 core\nlayout(location = 0) out vec4 color;\nvoid main(){color = vec4(1.0, 0.0, 0.0, 1.0);}");

	if(shader == 0 || shader == nullptr) {
		std::cerr << "[Shader Error] NULL" << std::endl;
		//exit(EXIT_FAILURE);
	}
	return shader;
}

//----------------Verification de Changement de Monde

void ChangeWorld(bool& isZonePortail) {

	isZonePortail = !isZonePortail;

}

//test de collision avec l'interieur du portail
bool isOnZonePortal(Camera& camera) {

	const float maxDistx = 0.5f;
	const float maxDisty = 0.5f;
	const float maxDistz = 0.1f;

	glm::vec3 cameraPos = camera.getPosition();

	if(cameraPos.x > -maxDistx && cameraPos.x < maxDistx
	        && cameraPos.y > -maxDisty && cameraPos.y < maxDisty
	        && cameraPos.z > -maxDistz && cameraPos.z < maxDistz)
		return true;
	else
		return false;

}

void TestChangeWorld(Camera& camera, bool& isOnPortal, bool& isOnIle) {

	if(!isOnPortal && isOnZonePortal(camera)) {
		isOnPortal = true;
		ChangeWorld(isOnIle);
	} else if(isOnPortal && !isOnZonePortal(camera)) {
		isOnPortal = false;
	}

}

//---------------------Animation camera --------

void bougerCamera(float &currParentRotation, float &currPosition, glm::mat4 &originalPos, glm::mat4 &cameraView, glm::mat4 &cameraParent, Camera &camera) {

	if (currParentRotation > 179.0f && currParentRotation < 181.0f) {
		//bouge vers le portail
		if (currPosition < 0.0f) {
			cameraView = glm::translate(cameraView, glm::vec3(0.0f, 0.0f, 0.1f));

			glm::mat4 finalView = cameraView * cameraParent;

			camera.SetView(finalView);
			currPosition += 0.1f;
		}
		//s'eloigne du portail
		else if (currPosition < 12.0f) {
			cameraView = glm::translate(cameraView, glm::vec3(0.0f, 0.0f, 0.1f));

			glm::mat4 finalView = cameraView;

			camera.SetView(finalView);

			currPosition += 0.1f;

		}
		//changement de monde
		else {

			cameraView = originalPos;

			glm::mat4 finalView = cameraView * cameraParent;

			camera.SetView(finalView);

			currPosition = -12;
			currParentRotation = 0;

		}

	}
	//tourne autour du portail
	else {

		cameraParent = glm::rotate(cameraParent, glm::radians(0.5f), glm::vec3(0.0f, -1.0f, 0.0f));

		glm::mat4 finalView = cameraView * cameraParent;

		camera.SetView(finalView);

		currParentRotation += 0.5f;
	}


}

//---------------------------Affichage effet Portail

//Limite du stencil: remplace aussi les pixels devant (meileure methode est camera to Texture)

void drawPortailDelete(RenderedObject& intPortail, RenderedObject& portail, float& currentTime, Camera& camera, Light& sun) {

	std::stack<glm::mat4> matrices;
	{	
		//Comme les propriétées du portail n'est pas la meme mais ils héritent du meme objet on pourrait faire la multiplication a la main ici

		glDisable(GL_STENCIL_TEST);

		portail.AfficherRecursif(matrices, currentTime, camera, sun);
	}
	{
		matrices = std::stack<glm::mat4>();

		glEnable(GL_STENCIL_TEST);

		// Draw floor
		glStencilFunc(GL_ALWAYS, 2, 0xFF); // Set any stencil to 1

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glStencilMask(0xFF); // Write to stencil buffer

		glDepthMask(GL_FALSE); // Don't write to depth buffer

		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

		intPortail.AfficherRecursif(matrices, currentTime, camera, sun);

		glDepthMask(GL_TRUE); // write to depth buffer

	}
}

void drawPortail(RenderedObject& intPortail, RenderedObject& portail, float& currentTime, Camera& camera, Light& sun) {

	std::stack<glm::mat4> matrices;
	{
		//Comme les propriétées du portail n'est pas la meme mais ils héritent du meme objet on pourrait faire la multiplication a la main ici

		glDisable(GL_STENCIL_TEST);

		portail.AfficherRecursif(matrices, currentTime, camera, sun);
	}
	{
		matrices = std::stack<glm::mat4>();

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1



		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glStencilMask(0xFF); // Write to stencil buffer

		glDepthMask(GL_FALSE); // Don't write to depth buffer

		glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

		intPortail.AfficherRecursif(matrices, currentTime, camera, sun);

		glDepthMask(GL_TRUE); // write to depth buffer

	}
}

void drawCurrentWorld(RenderedObject& r, float& currentTime, Camera& camera, Light& sun, Texture* caustic = nullptr) {
	std::stack<glm::mat4> matrices;

	//-------- Affichage normal
	/*
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_DEPTH_TEST);//Attention si il y a une erreur il faut penser a le changer

	glDepthMask(GL_TRUE); // Write to depth buffer

	glStencilMask(0x00); // Don't write anything to stencil buffer

	glEnable(GL_DEPTH_TEST);//Attention si il y a une erreur il faut penser a le changer

	glDepthMask(GL_TRUE); // Write to depth buffer
	*/

	//----Suppression image derriere

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 2, 0xFF); // Pass test if stencil value is 1


	glStencilMask(0x00); // Don't write anything to stencil buffer

	glEnable(GL_DEPTH_TEST);//Attention si il y a une erreur il faut penser a le changer

	glDepthMask(GL_TRUE); // Write to depth buffer


	r.AfficherRecursif(matrices, currentTime, camera, sun, caustic);

}


void drawOtherWorld(RenderedObject& r, float& currentTime, Camera& camera, Light& sun, Texture* caustic = nullptr) {


	std::stack<glm::mat4> matrices;

	glEnable(GL_STENCIL_TEST);
	// Draw
	glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1


	glStencilMask(0x00); // Don't write anything to stencil buffer

	glEnable(GL_DEPTH_TEST);//Attention si il y a une erreur il faut penser a le changer

	glDepthMask(GL_TRUE); // Write to depth buffer

	r.AfficherRecursif(matrices, currentTime, camera, sun, caustic);

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

void ActiverFog(glm::vec3 fogColor) {
	//------Fog
	glClearColor(fogColor.x, fogColor.y, fogColor.z, 1.0f);

	glClearDepth(1);

	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!



	glEnable(GL_FOG);
	GLfloat fogcolor[4] = { 0.5, 0.5, 0.5, 1 };
	GLint fogmode = GL_EXP;
	glFogi(GL_FOG_MODE, fogmode);
	glFogfv(GL_FOG_COLOR, fogcolor);
	glFogf(GL_FOG_DENSITY, 0.35);
	glFogf(GL_FOG_START, 1);
	glFogf(GL_FOG_END, 5);
	//----------/Fog


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
		std::cout << "stencil bit size (should be 8) : " << stencilBits << std::endl;

		float currParentRotation = 0.0f;
		float currPosition = 0.0f;
		glm::mat4 originalPos;

		bool isInPortail = true;
		bool isWorldIle = true;


		bool isWireframe = false;
		bool isCameraFree = false;

		glm::vec3 fogColor(0.6, 0.7, 0.95);

		RenderedObject root;
		Camera camera;

		//Creation des materiaux

		glm::vec4 matColor(1.0f, 1.0f, 1.0f, 1.0f);

		glm::vec4 propert(0.5f, 0.5f, 0.5f, 50.0f);
		glm::vec4 propert2(0.3f, 0.3f, 0.3f, 45.0f);

		Material defaultMat(matColor, propert);

		glm::vec4 troncColor(0.5f, 0.2f, 0.1f, 1.0f);
		Material troncMat(troncColor, propert2);

		glm::vec4 feuilleColor(0.35f, 0.6f, 0.1f, 1.0f);
		Material feuilleMat(feuilleColor, propert2);

		glm::vec4 rocherColor(0.4f, 0.4f, 0.5f, 1.0f);
		Material rocherMat(rocherColor, propert);

		glm::vec4 intPortailColor(0.4f, 0.1f, 0.2f, 0.2f);
		Material intPortailMat(intPortailColor, propert);

		//Poser la lumiere

		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec4 lightPos(0.0f, 1.0f, 0.0f, 1.0f);
		Light sun = Light(lightPos, lightColor);

		//Creation des Shaders

		auto texShader = setupShaders("resources/Shaders/Tex.vert", "resources/Shaders/Tex.frag", 2, "v_Position", "v_UV");

		auto lightTexShader = setupShaders("resources/Shaders/LightTex.vert", "resources/Shaders/LightTex.frag", 3, "v_Position", "v_UV", "v_Normal");

		auto underwaterShader = setupShaders("resources/Shaders/UnderW.vert", "resources/Shaders/UnderW.frag", 3, "v_Position", "v_UV", "v_Normal");

		auto causticWaterShader = setupShaders("resources/Shaders/UnderW.vert", "resources/Shaders/Caus.frag", 3, "v_Position", "v_UV", "v_Normal");

		//Creation des textures

		Texture emptyTexture("resources/img/.png");

		Texture skyboxTexture("resources/img/skybox.png");
		Texture ileTexture("resources/img/ile.png");
		Texture papillonTexture("resources/img/papillon.png");
		Texture rocherCorauxTexture("resources/img/rocher_coraux.png");
		Texture algesCoraux("resources/img/alges_coraux.png");
		Texture fondTexture("resources/img/fond.png");
		Texture portailTexture("resources/img/portail.png");
		Texture troncTexture("resources/img/tronc.png");

		Texture oiseauBodyTexture("resources/img/oiseauCorp.png");
		Texture oiseauAileTexture("resources/img/oiseauAile.png");

		Texture corpsBalaineTexture("resources/img/baleine/body.png");
		Texture aileBalaine1Texture("resources/img/baleine/wing1.png");
		Texture aileBalaine2Texture("resources/img/baleine/wing2.png");
		Texture queue1BalaineTexture("resources/img/baleine/tail1.png");
		Texture queue2BalaineTexture("resources/img/baleine/tail2.png");

		Texture poissonTexture("resources/img/poisson.png");

		Texture memeTexture("resources/img/meme/meme.png");
		Texture memeTexture2("resources/img/meme/meme2.png");

		Texture* caustic[] = {
			new Texture("resources/img/caus/save1.png"),
			new Texture("resources/img/caus/save2.png"),
			new Texture("resources/img/caus/save3.png"),
			new Texture("resources/img/caus/save4.png"),
			new Texture("resources/img/caus/save5.png"),
			new Texture("resources/img/caus/save6.png"),
			new Texture("resources/img/caus/save7.png"),
			new Texture("resources/img/caus/save8.png"),
			new Texture("resources/img/caus/save9.png"),
			new Texture("resources/img/caus/save10.png"),
			new Texture("resources/img/caus/save11.png"),
			new Texture("resources/img/caus/save12.png"),
			new Texture("resources/img/caus/save13.png"),
			new Texture("resources/img/caus/save14.png"),
			new Texture("resources/img/caus/save15.png"),
			new Texture("resources/img/caus/save16.png"),
		};


		//---------------------------- Camera -----------------------

		glm::mat4 cameraParent = glm::mat4(1.0f);

		glm::mat4 cameraView = camera.getViewM();

		cameraView = glm::translate(cameraView, glm::vec3(0.0f, 0.0f, -12.0f));

		currPosition = -12.0f;

		cameraParent = glm::rotate(cameraParent, glm::radians(-10.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

		originalPos = cameraView;

		//---------------------------- Sous Marin ------------------------------------



		RenderedObject PARENT_SOUS_MARIN(root);


		underwaterShader->Bind();

		VertexArray fondVA;
		Geometry* fondG = new ObjMesh("resources/Obj/fond.obj");
		ComplexVertexBuffer fondVB = fondG->bufferFactory();
		fondVA.addBuffer(fondVB, fondG->bufferLayoutFactory());
		RenderedObject Fond(fondVA, fondG, defaultMat, fondTexture, PARENT_SOUS_MARIN, causticWaterShader);
		{
			Fond.SetScale(glm::vec3(0.5f, 0.3f, 0.5f));
			Fond.Move(glm::vec3(-15.0f, 4.0f, -10.0f));
		}
		VertexArray rocherCorauxVA;
		Geometry* rocherCorauxG = new ObjMesh("resources/Obj/rock_coraux.obj");
		ComplexVertexBuffer rocherCorauxVB = rocherCorauxG->bufferFactory();
		rocherCorauxVA.addBuffer(rocherCorauxVB, rocherCorauxG->bufferLayoutFactory());
		RenderedObject rocherCoraux(rocherCorauxVA, rocherCorauxG, defaultMat, rocherCorauxTexture, Fond, underwaterShader);
		{
			rocherCoraux.Move(glm::vec3(20.0f, -9.0f, 16.0f));
			rocherCoraux.Rotate(-45, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		RenderedObject rocherCoraux2(rocherCorauxVA, rocherCorauxG, defaultMat, rocherCorauxTexture, Fond, underwaterShader);
		{
			rocherCoraux2.Move(glm::vec3(0.0f, -5.0f, 0.0f));
			rocherCoraux.Rotate(12, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		VertexArray jauneAlgesCorauxVA;
		Geometry* jauneAlgesCorauxG = new ObjMesh("resources/Obj/jaunes_coraux.obj");
		ComplexVertexBuffer jauneAlgesCorauxVB = jauneAlgesCorauxG->bufferFactory();
		jauneAlgesCorauxVA.addBuffer(jauneAlgesCorauxVB, jauneAlgesCorauxG->bufferLayoutFactory());
		RenderedObject jauneAlgesCoraux(jauneAlgesCorauxVA, jauneAlgesCorauxG, defaultMat, algesCoraux, rocherCoraux, underwaterShader);
		RenderedObject jauneAlgesCoraux2(jauneAlgesCorauxVA, jauneAlgesCorauxG, defaultMat, algesCoraux, rocherCoraux2, underwaterShader);

		VertexArray violetAlgesCoraux1VA;
		Geometry* violetAlgesCoraux1G = new ObjMesh("resources/Obj/roses1_coraux.obj");
		ComplexVertexBuffer violetAlgesCoraux1VB = violetAlgesCoraux1G->bufferFactory();
		violetAlgesCoraux1VA.addBuffer(violetAlgesCoraux1VB, violetAlgesCoraux1G->bufferLayoutFactory());
		RenderedObject violetAlgesCoraux1(violetAlgesCoraux1VA, violetAlgesCoraux1G, defaultMat, algesCoraux, rocherCoraux, underwaterShader);
		//RenderedObject violetAlgesCoraux12(violetAlgesCoraux1VA, violetAlgesCoraux1G, defaultMat, algesCoraux, rocherCoraux2, underwaterShader);

		VertexArray violetAlgesCoraux2VA;
		Geometry* violetAlgesCoraux2G = new ObjMesh("resources/Obj/roses2_coraux.obj");
		ComplexVertexBuffer violetAlgesCoraux2VB = violetAlgesCoraux2G->bufferFactory();
		violetAlgesCoraux2VA.addBuffer(violetAlgesCoraux2VB, violetAlgesCoraux2G->bufferLayoutFactory());
		RenderedObject violetAlgesCoraux2(violetAlgesCoraux2VA, violetAlgesCoraux2G, defaultMat, algesCoraux, rocherCoraux, underwaterShader);
		RenderedObject violetAlgesCoraux22(violetAlgesCoraux2VA, violetAlgesCoraux2G, defaultMat, algesCoraux, rocherCoraux2, underwaterShader);

		RenderedObject Parent_Oiseau(PARENT_SOUS_MARIN);
		{
			Parent_Oiseau.SetScale(glm::vec3(0.4f, 0.4f, 0.4f));

			Parent_Oiseau.AddAnimation(new Animation(0.0f, 50.0f, 0, new AnimRotateAction(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f)));
		}

		VertexArray oiseauCorpVA;
		Geometry* oiseauCorpG = new ObjMesh("resources/Obj/oiseauCorp.obj");
		ComplexVertexBuffer oiseauCorpVB = oiseauCorpG->bufferFactory();
		oiseauCorpVA.addBuffer(oiseauCorpVB, oiseauCorpG->bufferLayoutFactory());
		RenderedObject OiseauCorp(oiseauCorpVA, oiseauCorpG, defaultMat, oiseauBodyTexture, Parent_Oiseau, underwaterShader); {
			
			OiseauCorp.Move(glm::vec3(0.0f, 9.0f, 7.0f));
			//OiseauCorp.Rotate(90, glm::vec3(0, 1, 0));
		
		}




		VertexArray oiseauAileVA;
		Geometry* oiseauAileG = new ObjMesh("resources/Obj/oiseauAile.obj");
		ComplexVertexBuffer oiseauAileVB = oiseauAileG->bufferFactory();
		oiseauAileVA.addBuffer(oiseauAileVB, oiseauAileG->bufferLayoutFactory());
		RenderedObject OiseauAile1(oiseauAileVA, oiseauAileG, defaultMat, oiseauAileTexture, OiseauCorp, underwaterShader);
		{
			OiseauAile1.AddAnimation(new Animation(0.0f, 1000.0f, 1100, new AnimRotateAction(glm::vec3(1.0f, 0.0f, 0.0f), -0.6f)));
			OiseauAile1.AddAnimation(new Animation(1000.0f, 2000.0f, 1100, new AnimRotateAction(glm::vec3(1.0f, 0.0f, 0.0f), 0.6f)));
		}
		RenderedObject OiseauAile2(oiseauAileVA, oiseauAileG, defaultMat, oiseauAileTexture, OiseauCorp, underwaterShader);
		{
			OiseauAile2.SetScale(glm::vec3(1.0f, 1.0f, -1.0f));
			OiseauAile2.AddAnimation(new Animation(0.0f, 1000.0f, 1100, new AnimRotateAction(glm::vec3(1.0f, 0.0f, 0.0f), -0.6f)));
			OiseauAile2.AddAnimation(new Animation(1000.0f, 2000.0f, 1100, new AnimRotateAction(glm::vec3(1.0f, 0.0f, 0.0f), 0.6f)));
		}


		RenderedObject ParentBanc(PARENT_SOUS_MARIN); {

			ParentBanc.AddAnimation(new Animation(0.0f, 50.0f, 0, new AnimRotateAction(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f)));
		}

		RenderedObject BancPoisson(ParentBanc); {

			BancPoisson.Move(glm::vec3(3.0f, 3.0f, 0.0f));

			BancPoisson.SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
			BancPoisson.Rotate(90, glm::vec3(0, -1, 0));

			BancPoisson.AddAnimation(new Animation(0.0f, 2000.0f, 1500, new AnimMoveAction(glm::vec3(0.0f, 0.02f, 0.0f))));
			BancPoisson.AddAnimation(new Animation(2000.0f, 4000.0f, 1500, new AnimMoveAction(glm::vec3(0.0f, -0.02f, 0.0f))));


		}

		VertexArray poissonVA;
		Geometry* poissonG = new ObjMesh("resources/Obj/poisson.obj");
		ComplexVertexBuffer poissonVB = poissonG->bufferFactory();
		poissonVA.addBuffer(poissonVB, poissonG->bufferLayoutFactory());
		RenderedObject Poisson(poissonVA, poissonG, defaultMat, poissonTexture, BancPoisson, underwaterShader);
		{
			Poisson.Move(glm::vec3(0, 0, 0));
			Poisson.Rotate(90, glm::vec3(1, 0, 0));
		}
		RenderedObject Poisson2(poissonVA, poissonG, defaultMat, poissonTexture, BancPoisson, underwaterShader);
		{
			Poisson2.Move(glm::vec3(-1, 0, -2));
			Poisson2.Rotate(90, glm::vec3(1, 0, 0));
		}
		RenderedObject Poisson3(poissonVA, poissonG, defaultMat, poissonTexture, BancPoisson, underwaterShader);
		{
			Poisson3.Move(glm::vec3(0, 2, 0));
			Poisson3.Rotate(90, glm::vec3(1, 0, 0));
		}
		RenderedObject Poisson4(poissonVA, poissonG, defaultMat, poissonTexture, BancPoisson, underwaterShader);
		{
			Poisson4.Move(glm::vec3(0, -1, 2));
			Poisson4.Rotate(90, glm::vec3(1, 0, 0));
		}

		underwaterShader->Unbind();


		//--------------------------------- Portail -------------------------------

		lightTexShader->Bind();

		RenderedObject PARENT_PORTAIL(root);
		VertexArray portailVA;
		Geometry* portailG = new ObjMesh("resources/Obj/portail.obj");
		ComplexVertexBuffer portailVB = portailG->bufferFactory();
		portailVA.addBuffer(portailVB, portailG->bufferLayoutFactory());
		RenderedObject Portail(portailVA, portailG, defaultMat, portailTexture, PARENT_PORTAIL, lightTexShader);

		VertexArray intPortailVA;
		Geometry* intPortailG = new ObjMesh("resources/Obj/intPortail.obj");
		ComplexVertexBuffer intPortailVB = intPortailG->bufferFactory();
		intPortailVA.addBuffer(intPortailVB, intPortailG->bufferLayoutFactory());
		RenderedObject IntPortail(intPortailVA, intPortailG, intPortailMat, emptyTexture, PARENT_PORTAIL, lightTexShader);

		IntPortail.Rotate(90, glm::vec3(1, 0, 0));
		Portail.Rotate(90, glm::vec3(1, 0, 0));

		//------------------------------- Ile ---------------------------------

		RenderedObject PARENT_ILE(root);


		RenderedObject ParentPapillon(PARENT_ILE); {
			ParentPapillon.Move(glm::vec3(0.0f, 2.0f, 0.0f));
		}

		//Nous pourrions utiliser la meme animAction pour tous les papillons (et possiblement un Square)
		VertexArray papillonVA;
		Geometry* papillonG = new ObjMesh("resources/Obj/papillon.obj");

		ComplexVertexBuffer papillonVB = papillonG->bufferFactory();
		papillonVA.addBuffer(papillonVB, papillonG->bufferLayoutFactory());


		RenderedObject AileG(papillonVA, papillonG, defaultMat, papillonTexture, ParentPapillon, lightTexShader);
		{
			AileG.SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
			AileG.AddAnimation(new Animation(0.0f, 1000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
			AileG.AddAnimation(new Animation(1000.0f, 2000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
		}
		RenderedObject AileD(papillonVA, papillonG, defaultMat, papillonTexture, ParentPapillon, lightTexShader);
		{
			AileD.SetScale(glm::vec3(-0.1f, 0.1f, 0.1f));
			AileD.AddAnimation(new Animation(0.0f, 1000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
			AileD.AddAnimation(new Animation(1000.0f, 2000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
		}

		
		RenderedObject ParentPapillon2(PARENT_ILE); {
			ParentPapillon2.Move(glm::vec3(0.5f, 1.5f, 0.0f));

		}
		RenderedObject AileG2(papillonVA, papillonG, defaultMat, papillonTexture, ParentPapillon2, lightTexShader);
		{
			AileG2.SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
			AileG2.AddAnimation(new Animation(300.0f, 1300.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
			AileG2.AddAnimation(new Animation(1300.0f, 2300.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
		}
		RenderedObject AileD2(papillonVA, papillonG, defaultMat, papillonTexture, ParentPapillon2, lightTexShader);
		{
			AileD2.SetScale(glm::vec3(-0.1f, 0.1f, 0.1f));
			AileD2.AddAnimation(new Animation(700.0f, 1700.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
			AileD2.AddAnimation(new Animation(1700.0f, 2700.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
		}

		RenderedObject ParentPapillon3(PARENT_ILE); {
			ParentPapillon3.Move(glm::vec3(-0.5f, 1.5f, 0.5f));

		}
		RenderedObject AileG3(papillonVA, papillonG, defaultMat, papillonTexture, ParentPapillon3, lightTexShader);
		{
			AileG3.SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
			AileG3.AddAnimation(new Animation(0.0f, 1000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
			AileG3.AddAnimation(new Animation(1000.0f, 2000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
		}
		RenderedObject AileD3(papillonVA, papillonG, defaultMat, papillonTexture, ParentPapillon3, lightTexShader);
		{
			AileD3.SetScale(glm::vec3(-0.1f, 0.1f, 0.1f));
			AileD3.AddAnimation(new Animation(0.0f, 1000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
			AileD3.AddAnimation(new Animation(1000.0f, 2000.0f, 1100, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
		}
		
		papillonVB.Unbind();

		VertexArray ileVA;
		Geometry* ileG = new ObjMesh("resources/Obj/ile.obj");
		ComplexVertexBuffer ileVB = ileG->bufferFactory();
		ileVA.addBuffer(ileVB, ileG->bufferLayoutFactory());
		RenderedObject Ile(ileVA, ileG, defaultMat, ileTexture, PARENT_ILE, lightTexShader);
		{
			Ile.Move(glm::vec3(0, -3, 0));
			Ile.SetScale(glm::vec3(0.6f, 0.6f, 0.6f));
		}
		ileVB.Unbind();

		VertexArray troncVA;
		Geometry* troncG = new ObjMesh("resources/Obj/tronc.obj");
		ComplexVertexBuffer troncVB = troncG->bufferFactory();
		troncVA.addBuffer(troncVB, troncG->bufferLayoutFactory());
		RenderedObject Tronc(troncVA, troncG, troncMat, emptyTexture, Ile, lightTexShader);
		{
			Tronc.Move(glm::vec3(-3.0f, 0.0f, 3.0f));

		}
		troncVB.Unbind();


		//Si les rotations sont etranges, c'est du a un mauvais export blender
		VertexArray feuilleVA;
		Geometry* feuilleG = new ObjMesh("resources/Obj/feuille.obj");
		ComplexVertexBuffer feuilleVB = feuilleG->bufferFactory();
		feuilleVA.addBuffer(feuilleVB, feuilleG->bufferLayoutFactory());
		RenderedObject Feuille(feuilleVA, feuilleG, feuilleMat, emptyTexture, Tronc, lightTexShader);
		{
			Feuille.Move(glm::vec3(-5.5f, 12.7f, -2.4f));
			Feuille.Rotate(-30.0f, glm::vec3(1.0f, 0.0f, -1.0f));

		}
		RenderedObject Feuille2(feuilleVA, feuilleG, feuilleMat, emptyTexture, Tronc, lightTexShader);
		{
			Feuille2.Move(glm::vec3(-0.8f, 13.0f, 0.0f));
			Feuille2.Rotate(110.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Feuille2.Rotate(-30.0f, glm::vec3(1.0f, 0.0f, -0.6f));
		}
		RenderedObject Feuille3(feuilleVA, feuilleG, feuilleMat, emptyTexture, Tronc, lightTexShader);
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
		RenderedObject Rocher(rocherVA, rocherG, rocherMat, emptyTexture, Ile, lightTexShader);
		{
			Rocher.Move(glm::vec3(3.5f, -0.25f, 0));
		}
		rocherVB.Unbind();

		RenderedObject parentbalaine(PARENT_ILE);
		{
			parentbalaine.AddAnimation(new Animation(0.0f, 50.0f, 0, new AnimRotateAction(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f)));
		}
		VertexArray baleineCorpVA;
		Geometry* balaineCorpG = new ObjMesh("resources/Obj/baleine_corps.obj");
		ComplexVertexBuffer balaineCorpVB = balaineCorpG->bufferFactory();
		baleineCorpVA.addBuffer(balaineCorpVB, balaineCorpG->bufferLayoutFactory());
		RenderedObject BalaineCorp(baleineCorpVA, balaineCorpG, defaultMat, corpsBalaineTexture , parentbalaine, lightTexShader);
		{
			BalaineCorp.Move(glm::vec3(0.0f, 0.0f, -9.0f));
			BalaineCorp.SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		}
		balaineCorpVB.Unbind();

		VertexArray balaineAile1VA;
		Geometry* balaineAile1G = new ObjMesh("resources/Obj/baleine_aile1.obj");
		ComplexVertexBuffer balaineAile1VB = balaineAile1G->bufferFactory();
		balaineAile1VA.addBuffer(balaineAile1VB, balaineAile1G->bufferLayoutFactory());
		RenderedObject balaineAile1(balaineAile1VA, balaineAile1G, defaultMat, aileBalaine1Texture, BalaineCorp, lightTexShader);
		{
			balaineAile1.Move(glm::vec3(-2.0f, -2.0f, 2.0f));
			balaineAile1.AddAnimation(new Animation(0.0f, 450.0f, 450, new AnimRotateAction(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f)));
			balaineAile1.AddAnimation(new Animation(450.0f, 900.0f, 450, new AnimRotateAction(glm::vec3(1.0f, 0.0f, 0.0f), -1.0f)));
		}
		balaineAile1VB.Unbind();

		VertexArray balaineAile2VA;
		Geometry* balaineAile2G = new ObjMesh("resources/Obj/baleine_aile2.obj");
		ComplexVertexBuffer balaineAile2VB = balaineAile2G->bufferFactory();
		balaineAile2VA.addBuffer(balaineAile2VB, balaineAile2G->bufferLayoutFactory());
		RenderedObject balaineAile2(balaineAile2VA, balaineAile2G, defaultMat, aileBalaine2Texture, BalaineCorp, lightTexShader);
		{
			balaineAile2.Move(glm::vec3(-1.9f, -2.1f, -2.0f));
			balaineAile2.AddAnimation(new Animation(0.0f, 450.0f, 450, new AnimRotateAction(glm::vec3(1.0f, 0.0f, 0.0f), -1.0f)));
			balaineAile2.AddAnimation(new Animation(450.0f, 900.0f, 450, new AnimRotateAction(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f)));
		}
		balaineAile2VB.Unbind();

		VertexArray balaineQueue1VA;
		Geometry* balaineQueue1G = new ObjMesh("resources/Obj/baleine_queue1.obj");
		ComplexVertexBuffer balaineQueue1VB = balaineQueue1G->bufferFactory();
		balaineQueue1VA.addBuffer(balaineQueue1VB, balaineQueue1G->bufferLayoutFactory());
		RenderedObject balaineQueue1(balaineQueue1VA, balaineQueue1G, defaultMat, queue1BalaineTexture, BalaineCorp, lightTexShader);
		{
			balaineQueue1.Move(glm::vec3(4.7f, 0.4f, 0.0f));
			balaineQueue1.Rotate(20, glm::vec3(0.0f, 0.0f, 0.1f));
			balaineQueue1.AddAnimation(new Animation(0.0f, 600.0f, 600, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
			balaineQueue1.AddAnimation(new Animation(600.0f, 1200.0f, 600, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
		}
		balaineQueue1VB.Unbind();

		VertexArray balaineQueue2VA;
		Geometry* balaineQueue2G = new ObjMesh("resources/Obj/baleine_queue2.obj");
		ComplexVertexBuffer balaineQueue2VB = balaineQueue2G->bufferFactory();
		balaineQueue2VA.addBuffer(balaineQueue2VB, balaineQueue2G->bufferLayoutFactory());
		RenderedObject balaineQueue2(balaineQueue2VA, balaineQueue2G, defaultMat, queue2BalaineTexture, balaineQueue1, lightTexShader);
		{
			balaineQueue2.Move(glm::vec3(3.7f, 0.3f, 0.0f));
			balaineQueue2.Rotate(20, glm::vec3(0.0f, 0.0f, 0.1f));
			balaineQueue2.AddAnimation(new Animation(0.0f, 600.0f, 600, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), -1.0f)));
			balaineQueue2.AddAnimation(new Animation(600.0f, 1200.0f, 600, new AnimRotateAction(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f)));
		}
		balaineQueue2VB.Unbind();

		//Creation de la skybox

		VertexArray skyboxVA;
		Geometry* skyboxG = new ObjMesh("resources/Obj/skybox.obj");
		ComplexVertexBuffer skyboxVB = skyboxG->bufferFactory();
		skyboxVA.addBuffer(skyboxVB, skyboxG->bufferLayoutFactory());
		RenderedObject Skybox(skyboxVA, skyboxG, defaultMat, skyboxTexture, PARENT_ILE, texShader);
		{
			Skybox.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			Skybox.SetScale(glm::vec3(40.0f, 40.0f, 40.0f));
		}


		//------Easter Eggs -------

		VertexArray memeVA;
		Geometry* memeG = new Square();
		ComplexVertexBuffer memeVB = memeG->bufferFactory();
		memeVA.addBuffer(memeVB, memeG->bufferLayoutFactory());
		RenderedObject Meme(memeVA, memeG, defaultMat, memeTexture, PARENT_SOUS_MARIN, texShader);
		{
			Meme.Move(glm::vec3(0, -2.0f, 0.0f));
			Meme.Rotate(180, glm::vec3(0.0, 0.0, 1.0));
		}

		RenderedObject Meme2(memeVA, memeG, defaultMat, memeTexture2, PARENT_ILE, texShader);
		{
			Meme2.Move(glm::vec3(0, -2.0f, 0.0f));
			Meme2.Rotate(180, glm::vec3(0.0, 0.0, 1.0));
		}

		std::stack<glm::mat4> matrices;
		float currentTime = 0.0f;

		int frame = 0;
		int frameCount = 0;

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
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
					case SDLK_t://Met en mode WireFrame
						if(isWireframe)
							DesactiverWireframe();
						else
							ActiverWireframe();
						isWireframe = !isWireframe;
						break;
					case SDLK_f://Rend possible le mouvement de la camera avec clavier
						isCameraFree = !isCameraFree;
					}
				default:
					camera.inputMove(event);
				}
			}



			if(isCameraFree) {
				camera.UpdateView();//utilisateur bouge camera
			} else {
				bougerCamera(currParentRotation, currPosition, originalPos, cameraView, cameraParent, camera);//camera anime
			}

			//Verification si il passe dans le portail
			TestChangeWorld(camera, isInPortail, isWorldIle);


			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // don't forget to clear the stencil buffer!

			currentTime += TIME_PER_FRAME_MS;
			
			//Calcul de frame pour l'effet caustic
			frameCount++;
			
			if(frameCount % 2)
				frame++;
			if(frame >= 16)
				frame = 0;

			//Activation du brouillard (visible que en underWater car shader)
			ActiverFog(fogColor);

			//On affiche le portail qui servira a trier et supprimer les pixel via stencil
			drawPortailDelete(IntPortail, Portail, currentTime, camera, sun);

			drawCurrentWorld(isWorldIle ? PARENT_ILE : PARENT_SOUS_MARIN, currentTime, camera, sun, caustic[frame]);
			drawPortail(IntPortail, Portail, currentTime, camera, sun);
			drawOtherWorld(isWorldIle ? PARENT_SOUS_MARIN : PARENT_ILE, currentTime, camera, sun, caustic[frame]);

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
