#define _CRT_SECURE_NO_DEPRECATE

// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <sound/include/irrKlang.h>
#include "ComputeShader.hpp"
#include "Track.hpp"
#include "Tree.hpp"
#include "Flag.hpp"
#include "Fire.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include <glm/gtc/matrix_transform.hpp>

#if defined(WIN32)
#include <conio.h>
#else
#include "../common/conio.h"
#endif

using namespace irrklang;

Camera camera1(glm::vec3(0.0f, 30.0f, 0.0f));
Camera camera2(glm::vec3((0.0f, 0.0f, 0.0f)));
Camera CurrentCamera = camera2;
bool CurrentCamerais1 = false;
float deltaTime = 0.0f;
float mouseSpeed = 45.05f;
GLFWwindow* mWindow;
float splineScaling = 15.0f;
float splineHeight = 3.0f;

// Callbacks
static bool keys[1024];
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/);
static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/);
int CreateTexture(const char* namefile, const char* option = NULL);
int CreateSkybox(std::vector<std::string> skyboxTexture);
glm::vec2 CreateDepthMap(int SHADOW_WIDTH, int SHADOW_HEIGHT);
void UpdateCamera2Position();
void switchCameras();
void CreateTreePartsBuffers(Model bar, glm::mat4* modelMatrices, int amount);
void CreateParticleBuffers(Model particle, glm::mat4* modelMatrices, int amount);
void InitialiseWindow();
void GenerateSplineVAO(std::vector<GLfloat> Vec_vertices);
void CreateLoadingScreen();

void showFPS(void);
void Do_Movement();
void mouse_callback();
void MoveForward();
float g_CurrentTime = 0.0f;

int main(int argc, char * argv[]) {

	InitialiseWindow();

	// Music
	ISoundEngine* engine = createIrrKlangDevice();

	//Import textures
	GLuint texture = CreateTexture("brick.jpg");
	GLuint texture_normal = CreateTexture("NormalMap.jpg");
	GLuint texture_flag = CreateTexture("flag.png", "alpha");
	GLuint texture_phantom = CreateTexture("tissu.jpg");
	GLuint texture_skybox = CreateSkybox({ "posx.png","negx.png","posy.png","BlackS.jpg","posz.png","negz.png" }); 
	GLuint texture_metal = CreateTexture("metal.jpg");
	GLuint texture_speaker = CreateTexture("speaker.jpg");
	
	//Import models																											   //Import textures
	Model particle("particle.obj");
	Model cube("cubePlateform.obj");
	Model lamp("lamp.obj");
	Model ghost("ghost.obj");
	Model skybox_obj("cube.obj");
	Model bar("bar.obj");
	Model track("FinalTrack6.obj");
	Model trackSupport("FinalTrack67Support.obj");
	Model car("car4.obj");
	Model FlashCamera("FlashCamera.obj");
	Model Speaker("Speaker.obj");
	
	//Import shaders
	Shader SkyboxShader{ "SkyboxShader.vert","SkyboxShader.frag" };
	SkyboxShader.compile();
	Shader DefaultShader{ "DefaultShader.vert","DefaultShader.frag" };
	DefaultShader.compile();
	Shader DepthShader{ "ShadowShader.vert", "ShadowShader.frag" };
	DepthShader.compile();
	Shader DepthShaderTree{ "ShadowTreeShader.vert", "ShadowShader.frag" };
	DepthShaderTree.compile();
	Shader ParticleShader{ "ParticleShader.vert", "ParticleShader.frag" };
	ParticleShader.compile();
	Shader TissueShader{ "TissueShader.vert", "TissueShader.frag" };
	TissueShader.compile();
	Shader TreeShader{ "TreeShader.vert", "TreeShader.frag" };
	TreeShader.compile();
	Shader PhantomShader{ "PhantomShader.vert", "PhantomShader.frag" };
	PhantomShader.compile();
	ComputeShader myComputeShader{ "Compute.shader" };
	myComputeShader.compile();
	Shader TrackShader{ "TrackShader.vert", "TrackShader.frag" };
	TrackShader.compile();
	Shader CartShader{ "CartShader.vert", "CartShader.frag" };
	CartShader.compile();

	//Generate DepthMap (for shadows)
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glm::vec2 DepthVector = CreateDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT);
	unsigned int depthMapFBO = DepthVector[0];
	unsigned int depthMap = DepthVector[1];

	//Generate track
	std::vector<GLfloat> Vec_vertices = GenerateTrack("PrintPoints6.txt");
	GenerateSplineVAO(Vec_vertices);

	//Generate particules (for fire)
	unsigned int amount = 6000;
	float radius = 1.3;
	float offset = 0.8;
	glm::mat4* modelMatrices = CreateParticleModels(amount, radius, offset);
	CreateParticleBuffers(particle, modelMatrices, amount);

	//Generate flag
	GLuint size_flag = 40.0;
	GLuint totalParticles = size_flag * size_flag;

	glm::vec4* vertices = createFlagVertices(size_flag);
	glm::vec4* velocities = createFlagVelocities(size_flag);
	glm::vec2* uv = createFlagUV(size_flag);
	GLuint* indices = createFlagIndices(size_flag);
	GLuint* FlagBuffers = CreateFlagBuffers(vertices, velocities, totalParticles);
	GLuint* VAO_EBO = CreateFlagVAO(uv, indices, FlagBuffers[0], size_flag);
	GLuint VAO_tissue = VAO_EBO[0];
	GLuint EBO_tissue = VAO_EBO[1];

	//GenerateTree
	generateFractalTree(0.0, 0.0, 0.0, 30.0, 0.0, 0.0);
	glm::mat4* TreeModelMatrices = CreateTreePartsModels();
	CreateTreePartsBuffers(bar, TreeModelMatrices, TreePhi.size());
	
	//Update Cameras
	UpdateCamera2Position();
	switchCameras();
	ISound* music = engine->play3D("circus.wav", vec3df(0, 30.0, 20.0), true, false, true);
	if (music)
		music->setMinDistance(40.0f);
		music->setVolume(30.0);

#pragma region rendering
	// Rendering Loop
	while (glfwWindowShouldClose(mWindow) == false) {
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		showFPS();
		if (CurrentCamerais1) {
			mouse_callback();
			Do_Movement();
		}
		MoveForward();


		engine->setListenerPosition(vec3df(CurrentCamera.Position.x, CurrentCamera.Position.y, CurrentCamera.Position.z), vec3df(-CurrentCamera.Front.x, -CurrentCamera.Front.y, -CurrentCamera.Front.z));

		//////////////////////Set light, view matrix and projection matrix

		glm::vec3 lightPos = glm::vec3(-20.0f, 150.0f, -150.0f);
		glm::vec3 lightPosCamera = glm::vec3(-14.0, 48.0, -48.0);
		glm::mat4 View = CurrentCamera.GetViewMatrix();
		glm::mat4 Projection = glm::perspective(CurrentCamera.Zoom, (float)mWidth / (float)mHeight, 0.5f, 2000.0f);

		////////////////////////DRAW SKYBOX///////////////////////////

		SkyboxShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox);
		SkyboxShader.setInteger("skybox", 0);
		SkyboxShader.setMatrix4("projection", Projection);
		SkyboxShader.setMatrix4("view", View);
		skybox_obj.Draw(SkyboxShader);
		glDepthMask(GL_TRUE); // Re enable depth writing
		

		////////////////////////DRAW TREE/////////////////////////////

		TreeShader.use();
		TreeShader.setMatrix4("projection", Projection);
		TreeShader.setMatrix4("view", View);
		TreeShader.setVector3f("myLight", lightPos);
		TreeShader.setVector3f("camPosition", CurrentCamera.Position);

		glBindVertexArray(bar.getMeshes()[0].getVAO());
		glDrawElementsInstanced(GL_TRIANGLES, bar.getMeshes()[0].indices.size(), GL_UNSIGNED_INT, 0, TreePhi.size());
		glBindVertexArray(0);


		///////////////////GENERATE DEPTHMAP/////////////////////////
		
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

		DepthShader.use();

		float near_plane = -200, far_plane = 420;
		glm::mat4 lightProjection = glm::ortho(-120.0f, 120.0f, -120.0f, 120.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView; // render scene from light's point of view
		DepthShader.setMatrix4("lightSpaceMatrix", lightSpaceMatrix);
		
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		glm::mat4 biaslightSpaceMatrix = biasMatrix * lightSpaceMatrix;


		//Draw lamp (without fire)
		glm::mat4 lamp_model11 = glm::scale(glm::mat4(1.0), glm::vec3(5.0f, 4.0f, 5.0f));
		glm::mat4 lamp_model12 = glm::translate(glm::mat4(1.0), glm::vec3(10.0f, 3.0f, 10.0f));
		glm::mat4 lamp_model1 = lamp_model12 * lamp_model11;
		DepthShader.setMatrix4("model", lamp_model1);
		lamp.Draw(DepthShader);

		//Draw lamp (without fire)
		glm::mat4 lamp_model21 = glm::scale(glm::mat4(1.0), glm::vec3(5.0f, 4.0f, 5.0f));
		glm::mat4 lamp_model22 = glm::translate(glm::mat4(1.0), glm::vec3(10.0f, 3.0f, -10.0f));
		glm::mat4 lamp_model2 = lamp_model22 * lamp_model21;
		DepthShader.setMatrix4("model", lamp_model2);
		lamp.Draw(DepthShader);

		//Draw Car
		float s = 7.0f;

		glm::mat4 car_model1 = glm::scale(glm::mat4(1.0), glm::vec3(s, s, s));
		
		int index = CpLIndex - 30;
		if (CpLIndex - 30 < 0) {
			index = pLIndex + CpLIndex - 30;
		}
		float RotAngley = -angley(pointsList[index].normal, pointsList[0].normal);// +glm::radians(90.0f);
		glm::mat4 car_model3 = glm::rotate(glm::mat4(1.0), RotAngley, glm::vec3(0, 1, 0));//pointsList[0].biNormal.x, pointsList[0].biNormal.y, pointsList[0].biNormal.z));
		int index2 = CpLIndex;
		if (CpLIndex < 800) {
			index2 = CpLIndex + 30;
		}
		float RotAnglez = -anglez(pointsList[index2].biNormal, pointsList[0].biNormal);// +glm::radians(90.0f);
		glm::mat4 car_model4 = glm::rotate(glm::mat4(1.0), RotAnglez, glm::vec3(0, 0, 1));//pointsList[0].normal.x, pointsList[0].normal.y, pointsList[0].normal.z));
		
		glm::mat4 car_model5 = glm::translate(glm::mat4(1.0), glm::vec3(splineScaling * pointsList[CpLIndex].position.x + 3 * pointsList[CpLIndex].biNormal.x + pointsList[CpLIndex].normal.x, splineHeight + splineScaling * pointsList[CpLIndex].position.y + 3 * pointsList[CpLIndex].biNormal.y + pointsList[CpLIndex].normal.y, splineScaling * pointsList[CpLIndex].position.z + pointsList[CpLIndex].normal.z));

		glm::mat4 car_model = car_model5 * car_model4 * car_model3 * car_model1;// * car_model2

		DepthShader.setMatrix4("model", car_model);
		car.Draw(DepthShader);

		//Draw tracks	
		glm::mat4 track_model1 = glm::scale(glm::mat4(1.0), glm::vec3(splineScaling, splineScaling, splineScaling));
		glm::mat4 track_model2 = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, splineHeight, 0.0f));
		glm::mat4 track_model = track_model2 * track_model1;
		DepthShader.setMatrix4("model", track_model);
		track.Draw(DepthShader);

		//Draw Track support
		glm::mat4 trackSupport_model1 = glm::scale(glm::mat4(1.0), glm::vec3(splineScaling, splineScaling, splineScaling));
		glm::mat4 trackSupport_model2 = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, splineHeight, 0.0f));
		glm::mat4 trackSupport_model = trackSupport_model2 * trackSupport_model1;

		DepthShader.setMatrix4("model", trackSupport_model);
		trackSupport.Draw(DepthShader);

		//Draw FlashCamera (bar)
		glm::mat4 Cambar_model1 = glm::scale(glm::mat4(1.0), glm::vec3(0.3f, 25.0f, 0.3f));
		glm::mat4 Cambar_model2 = glm::translate(glm::mat4(1.0), glm::vec3(-45.0f, 0.0f, -35.0f));
		glm::mat4 Cambar_model = Cambar_model2 * Cambar_model1;
		DepthShader.setMatrix4("model", Cambar_model);
		bar.Draw(DepthShader);

		//Draw FlashCamera (camera)
		glm::mat4 camera_model1 = glm::scale(glm::mat4(1.0), glm::vec3(8.0f, 8.0f, 8.0f));
		glm::mat4 camera_model2 = glm::rotate(glm::mat4(1.0), glm::radians(-15.0f),glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 camera_model3 = glm::rotate(glm::mat4(1.0), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 camera_model4 = glm::translate(glm::mat4(1.0), glm::vec3(-45.0f, 50.0f, -35.0f));
		glm::mat4 camera_model = camera_model4 * camera_model3 * camera_model2 * camera_model1;
		DepthShader.setMatrix4("model", camera_model);
		FlashCamera.Draw(DepthShader);

		//Draw flag (bar)
		glm::mat4 bar_model = glm::translate(glm::mat4(1.0), glm::vec3(50.0f, 0.0f, 10.0f));
		bar_model = glm::scale(bar_model, glm::vec3(0.3f, 50.0f, 0.3f));
		DepthShader.setMatrix4("model", bar_model);
		bar.Draw(DepthShader);

		//Draw flag (tissue)
		glm::mat4 flag_model = glm::translate(glm::mat4(1.0), glm::vec3(50.0, 50.0, 10.0));
		DepthShader.setMatrix4("model", flag_model);
		glBindVertexArray(VAO_tissue);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_tissue);
		glDrawElements(GL_TRIANGLES, (size_flag - 1)*(size_flag - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		
		//Draw speaker 
		glm::mat4 speaker_model2 = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 20.0f));
		glm::mat4 speaker_model1 = glm::scale(glm::mat4(1.0), glm::vec3(4.0f, 4.0f, 4.0f));
		glm::mat4 speaker_model = speaker_model2 * speaker_model1;
		DepthShader.setMatrix4("model", speaker_model);
		Speaker.Draw(DepthShader);

		
		//Draw tree
		DepthShaderTree.use();
		DepthShaderTree.setMatrix4("lightSpaceMatrix", lightSpaceMatrix);
		glBindVertexArray(bar.getMeshes()[0].getVAO());
		glDrawElementsInstanced(GL_TRIANGLES, bar.getMeshes()[0].indices.size(), GL_UNSIGNED_INT, 0, TreePhi.size());
		glBindVertexArray(0);


		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, mWidth, mHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		
		///////////////////DRAW SCENE/////////////////////////
		
		DefaultShader.use();
		DefaultShader.setVector3f("myLight", lightPos);
		DefaultShader.setMatrix4("view", View);
		DefaultShader.setMatrix4("projection", Projection);
		DefaultShader.setVector3f("camPosition", CurrentCamera.Position);
		DefaultShader.setMatrix4("biaslightSpaceMatrix", biaslightSpaceMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		DefaultShader.setInteger("myTexture", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_normal);
		DefaultShader.setInteger("NormalMap", 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox);
		DefaultShader.setInteger("skybox", 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		DefaultShader.setInteger("depth", 3);

		// Draw Platform
		float transX = -60.0f;
		float transZ = -50.0f;

		glm::mat4 cube_model_S = glm::scale(glm::mat4(1.0), glm::vec3(3.0f, 2.0f, 3.0f));
		glm::mat4 cube_model1 = glm::translate(glm::mat4(1.0), glm::vec3(transX, 0.0f, transZ));
		glm::mat4 cube_model = cube_model1 * cube_model_S;
		DefaultShader.setMatrix4("model", cube_model);
		cube.Draw(DefaultShader);
		cube_model1 = glm::translate(glm::mat4(1.0), glm::vec3(120.0f + transX, 0.0f, transZ));
		cube_model = cube_model1 * cube_model_S;
		DefaultShader.setMatrix4("model", cube_model);
		cube.Draw(DefaultShader);
		cube_model1 = glm::translate(glm::mat4(1.0), glm::vec3(120.0f + transX, 0.0f, 120.0f + transZ));
		cube_model = cube_model1 * cube_model_S;
		DefaultShader.setMatrix4("model", cube_model);
		cube.Draw(DefaultShader);
		cube_model1 = glm::translate(glm::mat4(1.0), glm::vec3(transX, 0.0f, 120.0f + transZ));
		cube_model = cube_model1 * cube_model_S;
		DefaultShader.setMatrix4("model", cube_model);
		cube.Draw(DefaultShader);
		DefaultShader.setMatrix4("model", cube_model);
		cube.Draw(DefaultShader);

		DefaultShader.setMatrix4("model", lamp_model1);
		lamp.Draw(DefaultShader);

		DefaultShader.setMatrix4("model", lamp_model2);
		lamp.Draw(DefaultShader);

		DefaultShader.setMatrix4("model", bar_model);
		bar.Draw(DefaultShader);

		
		///////////////////DRAW FIRE/////////////////////////

		ParticleShader.use();
		ParticleShader.setMatrix4("view", View);
		ParticleShader.setMatrix4("projection", Projection);
		float time = glfwGetTime();
		ParticleShader.setFloat("time", time);
		ParticleShader.setVector3f("translation", glm::vec3(18.5f, 25.0f, -10.0f));

		glEnable(GL_CULL_FACE);

		glBindVertexArray(particle.getMeshes()[0].getVAO());
		glDrawElementsInstanced(GL_TRIANGLES, particle.getMeshes()[0].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);

		ParticleShader.setVector3f("translation", glm::vec3(18.5f, 25.0f, 10.0f));

		glBindVertexArray(particle.getMeshes()[0].getVAO());
		glDrawElementsInstanced(GL_TRIANGLES, particle.getMeshes()[0].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);

		glDisable(GL_CULL_FACE);


		///////////////////////DRAW GHOST//////////////////////////

		PhantomShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_phantom);
		PhantomShader.setInteger("Texture", 0);
		PhantomShader.setMatrix4("projection", Projection);
		PhantomShader.setMatrix4("view", View);
		PhantomShader.setVector3f("myLight", lightPos);
		PhantomShader.setVector3f("camPosition", CurrentCamera.Position);

		float radius = 200.0;
		float scal = cos(2*time)*0.05 + 0.7;
		glm::mat4 phantom_model1a = glm::scale(glm::mat4(1.0), glm::vec3(scal * 9.0f, scal * 15.0f, scal * 9.0f));
		glm::mat4 phantom_model1b = glm::rotate(glm::mat4(1.0), time / 10.0f - 90.0f / 180.0f*3.1416f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 phantom_model1c = glm::translate(glm::mat4(1.0), glm::vec3(radius*cos(time / 10.0f), 30.0f + sin(time) * 20, radius*sin(-time / 10.0f) + 15.0f));
		glm::mat4 phantom_model2b = glm::rotate(glm::mat4(1.0), time / 10.0f + 90.0f / 180.0f*3.1416f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 phantom_model2c = glm::translate(glm::mat4(1.0), glm::vec3(radius*cos(time / 10.0f + 3.1416f), 30.0f + sin(time + 3.14f / 2.0f) * 20, radius*sin(-time / 10.0f + 3.1416f) + 15.0f));


		glm::mat4 phantom_model1 = phantom_model1c * phantom_model1b * phantom_model1a;
		glm::mat4 phantom_model2 = phantom_model2c * phantom_model2b * phantom_model1a;
		PhantomShader.setMatrix4("model", phantom_model1);
		ghost.Draw(PhantomShader);
		PhantomShader.setMatrix4("model", phantom_model2);
		ghost.Draw(PhantomShader);
		
		///////////////////////DRAW TRACKS//////////////////////////
		
		TrackShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_metal);
		TrackShader.setInteger("Texture", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox);
		CartShader.setInteger("skybox", 1);
		TrackShader.setMatrix4("projection", Projection);
		TrackShader.setMatrix4("view", View);
		TrackShader.setVector3f("myLight", lightPos);
		TrackShader.setVector3f("camPosition", CurrentCamera.Position);
		TrackShader.setVector3f("LightCamera", lightPosCamera);

		float PowerCamera = 0.0f;
		if (CpLIndex > 7900 && CpLIndex < 7950) {
			PowerCamera = 2000.0f;
		}
		TrackShader.setFloat("LightPowerCamera", PowerCamera);

		TrackShader.setMatrix4("model", track_model);
		track.Draw(TrackShader);

		TrackShader.setMatrix4("model", trackSupport_model);
		trackSupport.Draw(TrackShader);

		TrackShader.setMatrix4("model", Cambar_model);
		bar.Draw(TrackShader);

		TrackShader.setMatrix4("model", camera_model);
		FlashCamera.Draw(TrackShader);

		

		/////////////////////////////DRAW SPEAKER///////////////////////////////////

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_speaker);
		TrackShader.setInteger("Texture", 0);
		TrackShader.setMatrix4("model", speaker_model);
		Speaker.Draw(TrackShader);

		///////////////////////DRAW CART//////////////////////////

		CartShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox);
		CartShader.setInteger("skybox", 0);
		CartShader.setMatrix4("projection", Projection);
		CartShader.setMatrix4("view", View);
		CartShader.setVector3f("myLight", lightPos);
		CartShader.setVector3f("LightCamera", lightPosCamera);
		CartShader.setVector3f("camPosition", CurrentCamera.Position);
		CartShader.setFloat("LightPowerCamera", PowerCamera);
		
		CartShader.setMatrix4("model", car_model);
		car.Draw(CartShader);

		
		////////////////////////DRAW FLAG/////////////////////////////

		//Compute displacement of flag

		myComputeShader.use();
		time = glfwGetTime();
		myComputeShader.setFloat("time", time);
		int ReadBuf = 0;
		for (int i = 0; i < 80; i++) {
			glDispatchCompute(size_flag / 20, size_flag / 20, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			ReadBuf = 1 - ReadBuf;

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1 - ReadBuf, FlagBuffers[0]);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ReadBuf, FlagBuffers[1]);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3 - ReadBuf, FlagBuffers[2]);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2 + ReadBuf, FlagBuffers[3]);
		}
		glUseProgram(0);


		//Draw flag

		TissueShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_flag);
		TissueShader.setInteger("Texture", 0);
		TissueShader.setMatrix4("projection", Projection);
		TissueShader.setMatrix4("view", View);
		TissueShader.setMatrix4("model", flag_model);

		glBindVertexArray(VAO_tissue);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_tissue);
		glDrawElements(GL_TRIANGLES, (size_flag - 1)*(size_flag - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
#pragma endregion

	if (music)
		music->drop(); 

	engine->drop(); 

	glfwTerminate();
	return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Functions used in the project


void showFPS(void) {
	static double lastTime = glfwGetTime();
	static int nbFrames = 0;

	// Measure speed
	double currentTime = glfwGetTime();
	nbFrames++;
	deltaTime = currentTime - lastTime;
	if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
										 // printf and reset timer
		std::cout << 1000.0 / double(nbFrames) << " ms/frame -> " << nbFrames << " frames/sec" << std::endl;
		nbFrames = 0;
		lastTime += 1.0;
	}
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;

	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);

	// V-SYNC
	if (keys[GLFW_KEY_U]) {
		static bool vsync = true;
		if (vsync) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}
		vsync = !vsync;
	}

	if ((keys[GLFW_KEY_0] || keys[GLFW_KEY_KP_0])) {
		std::cout << "You have pressed 0" << std::endl;
	}
	if (keys[GLFW_KEY_C]) {
		switchCameras();
	}

}

void MoveForward() {
	
	static double lastTime = glfwGetTime();// time;

	// Measure speed
	double currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	float slope = pointsList[CpLIndex].tangent.y;
	if (slope < 0) {
		CpLIndex += 500 * deltaTime - slope * 20;
	}
	else {
		CpLIndex += 500 * deltaTime - slope * 5;
	}

	if (CpLIndex >= pLIndex - 1) {
		CpLIndex -= pLIndex;
	}
	if (!CurrentCamerais1) {
		UpdateCamera2Position();
	}
}

void switchCameras() {
	if (CurrentCamerais1) {
		camera1 = CurrentCamera;
		CurrentCamera = camera2;
		CurrentCamerais1 = false;
		std::cout << "Camera 2 activated" << std::endl;
	}
	else {
		camera2 = CurrentCamera;
		CurrentCamera = camera1;
		CurrentCamerais1 = true;
		glfwSetCursorPos(mWindow, mWidth / 2, mHeight / 2); // Reset mouse position
		std::cout << "Camera 1 activated" << std::endl;
	}
}

static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_RIGHT] = true;
	else
		keys[GLFW_MOUSE_BUTTON_RIGHT] = false;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_LEFT] = true;
	else
		keys[GLFW_MOUSE_BUTTON_LEFT] = false;

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		keys[GLFW_MOUSE_BUTTON_MIDDLE] = true;
	else
		keys[GLFW_MOUSE_BUTTON_MIDDLE] = false;
}

void mouse_callback()// GLFWwindow* window, double xpos, double ypos)
{
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime); // Compute time difference between current and last frame

	double xpos, ypos;
	glfwGetCursorPos(mWindow, &xpos, &ypos); // Get mouse position

	GLfloat xoffset = mouseSpeed * deltaTime * float(xpos - mWidth / 2);
	GLfloat yoffset = mouseSpeed * deltaTime * float(mHeight / 2 - ypos);

	CurrentCamera.ProcessMouseMovement(xoffset, yoffset);
	glfwSetCursorPos(mWindow, mWidth / 2, mHeight / 2); // Reset mouse position for next frame
	lastTime = currentTime;
}

void Do_Movement() {
	if (keys[GLFW_KEY_W])    CurrentCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])    CurrentCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])    CurrentCamera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])    CurrentCamera.ProcessKeyboard(RIGHT, deltaTime);
}

int CreateTexture(const char* namefile, const char* option)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, n;
	unsigned char* image = stbi_load(namefile, &width, &height, &n, 0); 
	if (option == "alpha")
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glGenerateMipmap(GL_TEXTURE_2D); 
	stbi_image_free(image); 
	glBindTexture(GL_TEXTURE_2D, 0); 
	return texture;
}

int CreateSkybox(std::vector<std::string> skyboxTexture)
{
	GLuint texture_skybox;
	glGenTextures(1, &texture_skybox);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, n;
	unsigned char* image;
	for (GLuint i = 0; i < skyboxTexture.size(); i++) {
		image = stbi_load(skyboxTexture[i].c_str(), &width, &height, &n, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texture_skybox;
}

void CreateTreePartsBuffers(Model bar, glm::mat4* modelMatrices, int amount)
{
	unsigned int ParticleBuffer;
	glGenBuffers(1, &ParticleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);


	unsigned int VAO = bar.getMeshes()[0].getVAO();
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);

	glBindVertexArray(0);
}

glm::vec2 CreateDepthMap(int SHADOW_WIDTH, int SHADOW_HEIGHT)
{
	unsigned int depthMapFBO, depthMap;
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return glm::vec2(depthMapFBO, depthMap);
}

/*
setup the camera's position, where to look at and where the upvector is.
*/
void UpdateCamera2Position() {

	CurrentCamera.Position.x = splineScaling * pointsList[CpLIndex].position.x + 11 * pointsList[CpLIndex].biNormal.x + pointsList[CpLIndex].normal.x + 8.5 * pointsList[CpLIndex].tangent.x;
	CurrentCamera.Position.y = splineHeight + splineScaling * pointsList[CpLIndex].position.y + 11 * pointsList[CpLIndex].biNormal.y + pointsList[CpLIndex].normal.y + 8.5 * pointsList[CpLIndex].tangent.y;
	CurrentCamera.Position.z = splineScaling * (pointsList[CpLIndex].position.z) + 11 * pointsList[CpLIndex].biNormal.z + pointsList[CpLIndex].normal.z + 8.5 * pointsList[CpLIndex].tangent.z;

	CurrentCamera.Front.x = pointsList[CpLIndex].tangent.x;
	CurrentCamera.Front.y = pointsList[CpLIndex].tangent.y;
	CurrentCamera.Front.z = pointsList[CpLIndex].tangent.z;
}


void CreateParticleBuffers(Model particle, glm::mat4* modelMatrices, int amount)
{
	unsigned int ParticleBuffer;
	glGenBuffers(1, &ParticleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);


	unsigned int VAO = particle.getMeshes()[0].getVAO();
	glBindVertexArray(VAO);
	// set attribute pointers for matrix (4 times vec4)
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);

	glBindVertexArray(0);

	GLfloat *RandomValues;
	RandomValues = new GLfloat[amount];
	for (unsigned int i = 0; i < amount; i++)
	{
		RandomValues[i] = (rand() % 10) / 10.0;
	}
	unsigned int ParticleRandomBuffer;
	glGenBuffers(1, &ParticleRandomBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ParticleRandomBuffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(GLfloat), &RandomValues[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < particle.getMeshes().size(); i++)
	{
		unsigned int VAO = particle.getMeshes()[i].getVAO();
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)0);

		glVertexAttribDivisor(8, 1);
		glBindVertexArray(0);
	}
}

void InitialiseWindow() {
	// Load GLFW and Create a Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	mWindow = glfwCreateWindow(mWidth, mHeight, "CREEPY ROLLERCOASTER", nullptr, nullptr);

	// Check for Valid Context
	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(1);
	}

	// Create Context and Load OpenGL Functions
	glfwMakeContextCurrent(mWindow);
	gladLoadGL();
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Hides the mouse and enables unlimited mouvement
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Sets the mouse at the centre of the screen
	//glfwPollEvents();
	glfwSetCursorPos(mWindow, mWidth / 2, mHeight / 2);

	// Set the required callback functions
	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetMouseButtonCallback(mWindow, mouse_button_callback);

	//Defaults
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CreateLoadingScreen();

	// Flip Buffers and Draw
	glfwSwapBuffers(mWindow);
	glfwPollEvents();

}

void GenerateSplineVAO(std::vector<GLfloat> Vec_vertices) {


	// Construct a Line
	GLuint lineVBO, lineVAO;
	glGenVertexArrays(1, &lineVAO);
	glBindVertexArray(lineVAO);

	glGenBuffers(1, &lineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, Vec_vertices.size() * sizeof(GLfloat), &Vec_vertices[0], GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

}

void CreateLoadingScreen() {
	GLuint texture_loading = CreateTexture("LoadingS1.jpg", "alpha");

	Shader LoadingBarShader{ "LoadingBarShader.vert", "LoadingBarShader.frag" };
	LoadingBarShader.compile();

	const GLfloat LoadingBar_buffer_data[] = {
		// Coordinates		// UVs					//UVs
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, //     0.000f, 0.000f, 1.0f,		-1.0f, -1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f,//		0.000f, 0.000f, 1.0f,     1.0f, -1.0f,
		-1.0f,  -1.0f, 0.0f, 0.0f,  1.0f,//		0.000f, 0.000f, 1.0f,     1.0f,  1.0f,

		-1.0f,  -1.0f, 0.0f, 0.0f, 1.0f,//		0.000f, -2.000f, -4.0f,     -1.0f, -1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f,//		0.000f, -2.000f, -4.0f,     1.0f, 1.0f,
		1.0f,  -1.0f, 0.0f, 1.0f,  1.0f//,	0.000f, -2.000f, -4.0f,     -1.0f,  1.0f
	};

	// Construct a LoadingBar

	GLuint LoadingBarVBO, LoadingBarVAO;
	glGenVertexArrays(1, &LoadingBarVAO);
	glBindVertexArray(LoadingBarVAO);
	glGenBuffers(1, &LoadingBarVBO);
	glBindBuffer(GL_ARRAY_BUFFER, LoadingBarVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LoadingBar_buffer_data), LoadingBar_buffer_data, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Colour
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// Background Fill Color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//glDepthMask(GL_FALSE); // Remove depth writing

	LoadingBarShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_loading);
	LoadingBarShader.setInteger("loadingTexture", 0);
	glBindVertexArray(LoadingBarVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6); // draw loading bar
	glBindVertexArray(0);
}
