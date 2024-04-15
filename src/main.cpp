/*
 * Program 4 example with diffuse and spline camera PRESS 'g'
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn (spline D. McGirr)
 */

#include <iostream>
#include <glad/glad.h>
#include <algorithm>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"
#include "Entity.h"


#include <chrono>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>
#define PI 3.1415927

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program - use this one for Blinn-Phong has diffuse


	std::shared_ptr<Program> prog;           // 

	//Our shader program for textures
	std::shared_ptr<Program> texProg;

	//our geometry
	shared_ptr<Shape> sphere;

	std::vector<shared_ptr<Shape>> butterfly;

	Entity bf = Entity();

	std::vector<shared_ptr<Shape>> flower;

	std::vector<shared_ptr<Shape>> tree1;
	
	shared_ptr<Shape> cat;
	

	//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;

	//the image to use as a texture (ground)
	shared_ptr<Texture> texture0;
	shared_ptr<Texture> texture1;	
	shared_ptr<Texture> texture2;
	shared_ptr<Texture> texture3;

	//animation data
	float lightTrans = 0;
	float sTheta = 0;
	float cTheta = 0;
	float gTrans = 0;

	//camera
	double g_theta;
	vec3 view = vec3(0, 0, 1);
	vec3 strafe = vec3(1, 0, 0);
	vec3 g_eye = vec3(0, 0.5, 5);
	float pitch = 17;
	float dist = 4;
	float angle = 0;


	//player animation
	float player_rot = 0;
	vec3 player_pos = vec3(0, -1.12, 0);
	float oscillate = 0;

	//rules for cat walking around
	bool animate = false;
	bool back_up = false;
	
	//keyframes for cat walking animation
	double f[5][12] = {
			{0.5, -0.6, 0.1, -0.5, 1.0, 0.1, 0.5, -0.5, 0, 0, -0.58, 0.58},
			{0.5, 0, -0.4, -0.5, 0.45, 0.72, 0.45, -0.95, 0.72, 0, 0.1, 0.08},
			{0.65, -0.5, 0.7, -0.5, 0, 0.7, 0.2, -0.85, 0.75, 0.1, 0.1, 0.082},
			{0.4, -1.2, 1.3, 0.30, -0.3, 0, -0.2, -0.2, 0.1, 0.6, -0.6, 0},
			{-0.1, -0.45, 0.55, 0.30, 0.1, -0.3, 0.1, -0.2, 0.2, 0, -0.6, 0.6}
		};

	//bounding "cylinders" for flower & tree
	double flower_radial;
	double tree_radial;

	//bounds for world
	double bounds;
		
	//interpolation of keyframes for animation
	int cur_idx = 0, next_idx = 1;
	float frame = 0.0;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		animate = false;
		if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)&& !back_up){
			player_rot -= 10 * 0.01745329;
			animate = true;
		}
		if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT) && !back_up){
			player_rot += 10 * 0.01745329;
			animate = true;
		}
		if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT) && !back_up && bounds < 19){
			player_pos += vec3(sin(player_rot) * 0.1, 0, cos(player_rot) * 0.1);
			animate = true;
		}
		if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT) && bounds < 19){
			player_pos -= vec3(sin(player_rot) * 0.1, 0, cos(player_rot) * 0.1);
			animate = true;
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY) {
   		cout << "xDel + yDel " << deltaX << " " << deltaY << endl;
		angle -= 10 * (deltaX / 57.296);
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		g_theta = -PI/2.0;

		// Initialize the GLSL program that we will use for local shading
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("lightPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex"); //silence error



		// Initialize the GLSL program that we will use for texture mapping
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addUniform("flip");
		texProg->addUniform("Texture0");
		texProg->addUniform("MatShine");
		texProg->addUniform("MatSpec");
		texProg->addUniform("MatAmb");
		texProg->addUniform("lightPos");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

		//read in a load the texture
		texture0 = make_shared<Texture>();
  		texture0->setFilename(resourceDirectory + "/grass_tex.jpg");
  		texture0->init();
  		texture0->setUnit(0);
  		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

  		texture1 = make_shared<Texture>();
  		texture1->setFilename(resourceDirectory + "/sky.jpg");
  		texture1->init();
  		texture1->setUnit(1);
  		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

  		texture2 = make_shared<Texture>();
  		texture2->setFilename(resourceDirectory + "/cat_tex.jpg");
  		texture2->init();
  		texture2->setUnit(2);
  		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		texture3 = make_shared<Texture>();
  		texture3->setFilename(resourceDirectory + "/cat_tex_legs.jpg");
  		texture3->init();
  		texture3->setUnit(3);
  		texture3->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		
    
	}

	void initGeom(const std::string& resourceDirectory)
	{
		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/sphereWTex.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			sphere = make_shared<Shape>();
			sphere->createShape(TOshapes[0]);
			sphere->measure();
			sphere->init();
		}

		// Initialize cat mesh.
		vector<tinyobj::shape_t> TOshapesB;
 		vector<tinyobj::material_t> objMaterialsB;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesB, objMaterialsB, errStr, (resourceDirectory + "/cat.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {	
			cat = make_shared<Shape>();
			cat->createShape(TOshapesB[0]);
			cat->measure();
			cat->init();
		}

		vector<tinyobj::shape_t> TOshapes3;
		rc = tinyobj::LoadObj(TOshapes3, objMaterials, errStr, (resourceDirectory + "/butterfly.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//scan in all parts of butterfly
			for (int i = 0; i < 3; i++) {
				butterfly.push_back(make_shared<Shape>());
				butterfly[i]->createShape(TOshapes3[i]);
				butterfly[i]->measure();
				butterfly[i]->init();
			}
		}

		vector<tinyobj::shape_t> TOshapes4;
		rc = tinyobj::LoadObj(TOshapes4, objMaterials, errStr, (resourceDirectory + "/flower.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			for (int i = 0; i < 3; i++) {
				//scan in current obj part of flower
				flower.push_back(make_shared<Shape>());
				flower[i]->createShape(TOshapes4[i]);
				flower[i]->measure();
				flower[i]->init();
			}
		}

		//bounding cylinder of flower
		flower_radial = std::sqrt(
			(flower[2]->max.x - flower[2]->min.x) * (flower[2]->max.x - flower[2]->min.x)
			+ (flower[2]->max.z - flower[2]->min.z) * (flower[2]->max.z - flower[2]->min.z)
		) * 2.5;


		vector<tinyobj::shape_t> TOshapes5;
		rc = tinyobj::LoadObj(TOshapes5, objMaterials, errStr, (resourceDirectory + "/trees.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			//for now all our shapes will not have textures - change in later labs
			for (int i = 0; i < 12; i++) {
				//scan in current obj part of flower
				tree1.push_back(make_shared<Shape>());

				tree1[i]->createShape(TOshapes5[i]);
				tree1[i]->measure();
				tree1[i]->init();
			}
		}

		//bounding cylinder for trunk
		tree_radial = std::sqrt(
			(tree1[0]->max.x - tree1[0]->min.x) * (tree1[0]->max.x - tree1[0]->min.x)
			+ (tree1[0]->max.z - tree1[0]->min.z) * (tree1[0]->max.z - tree1[0]->min.z)
		);

		// init butterfly
		bf.initEntity(butterfly);
		bf.position = vec3(0.5, 0.2, 0.5);
		
		//code to load in the ground plane (CPU defined data passed to GPU)
		initGround();
	}

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = 60;
		float g_groundY = -0.25;

  		// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY, -g_groundSize,
			-g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		static GLfloat GrndTex[] = {
      		0, 0, // back
      		0, 1,
      		1, 1,
      		1, 0 };

      	unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		//generate the ground VAO
      	glGenVertexArrays(1, &GroundVertexArrayID);
      	glBindVertexArray(GroundVertexArrayID);

      	g_GiboLen = 6;
      	glGenBuffers(1, &GrndBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndNorBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndTexBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

      	glGenBuffers(1, &GIndxBuffObj);
     	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
      }



      //code to draw the ground plane
     void drawGround(shared_ptr<Program> curS) {
     	curS->bind();
     	glBindVertexArray(GroundVertexArrayID);
     	texture0->bind(curS->getUniform("Texture0"));
		
		glUniform1i(curS->getUniform("flip"), 1);
		glUniform3f(curS->getUniform("MatAmb"), 0.05, 0.22, 0.05);
		glUniform3f(curS->getUniform("MatSpec"), 3, 3, 3);
		glUniform1f(curS->getUniform("MatShine"), 1.0);
		//draw the ground plane 
  		SetModel(curS, vec3(0, -1, 0), 0, 0, 0, 1);
  		glEnableVertexAttribArray(0);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
  		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(1);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
  		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(2);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
  		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   		// draw!
  		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
  		glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

  		glDisableVertexAttribArray(0);
  		glDisableVertexAttribArray(1);
  		glDisableVertexAttribArray(2);
  		curS->unbind();
     }


	/* helper function to set model trasnforms */
  	void SetModel(shared_ptr<Program> curS, vec3 trans, float rotZ, float rotY, float rotX, float sc) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
		mat4 RotZ = glm::rotate( glm::mat4(1.0f), rotZ, vec3(0, 0, 1));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
  		mat4 ctm = Trans*RotX*RotY*RotZ*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   	}

	void SetView(shared_ptr<Program> shader) {
		float horiz = dist * cos(pitch * 0.01745329);   // for third person camera - calculate horizontal and
		float vert = dist * sin(pitch * 0.01745329);    // vertical offset based on maintained distance
		float offX = horiz * sin(angle);				// rotation around cat
		float offZ = horiz * cos(angle);

		g_eye = vec3(player_pos[0] - offX, player_pos[1] + vert, player_pos[2] - offZ);
  		glm::mat4 Cam = glm::lookAt(g_eye, player_pos, vec3(0, 1, 0));
  		glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
	}

	//used for checking collisions
	void check_collision(vec3 f_list[], int len1, vec3 t_list[], int len2, vec3 cat_pos) {
		//first check all flowers
		back_up = false;
		for (int i = 0; i < len1; i++) {
			float distance = std::sqrt(
				(f_list[i][0] - player_pos[0]) * (f_list[i][0] - player_pos[0])
				+ (f_list[i][2] - player_pos[2]) * (f_list[i][2] - player_pos[2])
			);
			distance = std::abs(distance);
			if (distance < flower_radial + 0.4) {
				back_up = true;
				return;
			}
		}
		//then check all trees
		for (int i = 0; i < len2; i++) {
			float distance = std::sqrt(
				(t_list[i][0] - player_pos[0]) * (t_list[i][0] - player_pos[0])
				+ (t_list[i][2] - player_pos[2]) * (t_list[i][2] - player_pos[2])
			);
			distance = std::abs(distance);
			if (distance < 0.9) {
				back_up = true;
				return;
			}
		}
	}


	void render(float frametime) {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();


		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		
		
		//material shader first
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(prog);

		float butterfly_height[3] = {1.1, 1.7, 1.5};

		
		vec3 butterfly_loc[3];
		butterfly_loc[0] = vec3(-2.3, -1, 3);
		butterfly_loc[1] = vec3(-2, -1.2, -3);
		butterfly_loc[2] = vec3(4, -1, 4);

		bf.setMaterials(0, 0.1, 0.1, 0.1, 0.02, 0.02, 0.02, 0.25, 0.23, 0.30, 9);
		bf.setMaterials(1, 0.4, 0.2, 0.2, 0.94, 0.23, 0.20, 0.9, 0.23, 0.20, 0.6);
		bf.setMaterials(2, 0.4, 0.2, 0.2, 0.94, 0.23, 0.20, 0.9, 0.23, 0.20, 0.6);

		SetModel(prog, vec3(-0.8, butterfly_height[0] + abs(cTheta), 0.9) + butterfly_loc[0], -1.1, 4.1, 0, 0.01); //body
		for (int i = 0; i < 3; i++) {
			glUniform3f(prog->getUniform("MatAmb"), bf.material[i].matAmb.r, bf.material[i].matAmb.g, bf.material[i].matAmb.b);
			glUniform3f(prog->getUniform("MatDif"), bf.material[i].matDif.r, bf.material[i].matDif.g, bf.material[i].matDif.b);
			glUniform3f(prog->getUniform("MatSpec"), bf.material[i].matSpec.r, bf.material[i].matSpec.g, bf.material[i].matSpec.b);
			glUniform1f(prog->getUniform("MatShine"), bf.material[i].matShine);
			butterfly[i]->draw(prog);
		}

		float butterfly_colors[3][6] {
			{0.3, 0.3, 0.2, 0.90, 0.73, 0.20},
			{0.2, 0.3, 0.3, 0.20, 0.73, 0.80},
			{0.4, 0.2, 0.2, 0.94, 0.23, 0.20}
		};

		for(int i = 0; i<3 ; i ++){
			SetModel(prog, bf.position, -0.8+(sTheta/2), 4.1+sTheta, 0.0, 0.01);
			butterfly[i]->draw(prog);
		}
		
		for (int i = 0; i < 2; i++) {
			SetModel(prog, vec3(-0.8, butterfly_height[i] + abs(cTheta), 0.9) + butterfly_loc[i], -1.1, 4.1, 0, 0.01); //body
			glUniform3f(prog->getUniform("MatAmb"), 0.1, 0.1, 0.1);
			glUniform3f(prog->getUniform("MatDif"), 0.02, 0.02, 0.02);
			glUniform3f(prog->getUniform("MatSpec"), 0.25, 0.23, 0.30);
			glUniform1f(prog->getUniform("MatShine"), 9);
			butterfly[0]->draw(prog);

			glUniform3f(prog->getUniform("MatAmb"), butterfly_colors[i][0], butterfly_colors[i][1], butterfly_colors[i][2]);
			glUniform3f(prog->getUniform("MatDif"), butterfly_colors[i][3], butterfly_colors[i][4], butterfly_colors[i][5]);
			glUniform3f(prog->getUniform("MatSpec"), 0.9, 0.23, 0.20);
			glUniform1f(prog->getUniform("MatShine"), 0.6);
											
			// body up and down     //downwards      //forward and back
			SetModel(prog, vec3(-0.8, butterfly_height[i] + abs(cTheta), 0.9) + butterfly_loc[i], -0.8+(sTheta/2), 4.1+sTheta, 0, 0.01); //left wing
			butterfly[1]->draw(prog);
			// // body up and down     //downwards      //forward and back
			SetModel(prog, vec3(-0.8, butterfly_height[i] + abs(cTheta), 0.9) + butterfly_loc[i], -0.8+(sTheta/2), 4.1-sTheta, 0, 0.01); //right wing
			butterfly[2]->draw(prog);
						
		}



		//where each flower will go
		vec3 flower_loc[7];
		flower_loc[0] = vec3(4, -1, 4);
		flower_loc[1] = vec3(-2.3, -1, 3);
		flower_loc[2] = vec3(-2, -1.2, -3);
		flower_loc[3] = vec3(4, -1, -3.2);
		flower_loc[4] = vec3(-5, -1, 2);
		flower_loc[5] = vec3(1, -1, -1.7);
		flower_loc[6] = vec3(3, -1, -2);


		// populate them all!!
		SetModel(prog, vec3(4, -1, 4), cTheta*cTheta, 0, 0, 2.5);
		for (int i = 0; i < 7; i++) {
			SetModel(prog, flower_loc[i], cTheta*cTheta+0.03, 0, 0, 2.5); 
			glUniform3f(prog->getUniform("MatAmb"), 0.2, 0.1, 0.1);          //pink petals
			glUniform3f(prog->getUniform("MatDif"), 0.94, 0.42, 0.64);
			glUniform3f(prog->getUniform("MatSpec"), 0.7, 0.23, 0.60);
			glUniform1f(prog->getUniform("MatShine"), 100);
			flower[0]->draw(prog); 
			glUniform3f(prog->getUniform("MatAmb"), 0.1, 0.1, 0.1);          //yellow center
			glUniform3f(prog->getUniform("MatDif"), 0.94, 0.72, 0.22);
			glUniform3f(prog->getUniform("MatSpec"), 0.23, 0.23, 0.20);
			glUniform1f(prog->getUniform("MatShine"), 100);
			flower[1]->draw(prog);
			glUniform3f(prog->getUniform("MatAmb"), 0.05, 0.15, 0.05);       //green stem & leaves
			glUniform3f(prog->getUniform("MatDif"), 0.24, 0.92, 0.41);
			glUniform3f(prog->getUniform("MatSpec"), 1, 1, 1);
			glUniform1f(prog->getUniform("MatShine"), 0);
			flower[2]->draw(prog);
		}

		// tree locations
		vec3 tree_loc[7];
		tree_loc[0] = vec3(4, -5.5, 7);
		tree_loc[1] = vec3(-2.9,-5.5, -7);
		tree_loc[2] = vec3(8, -5.5, -3);
		tree_loc[3] = vec3(6, -5.5, -3.7);
		tree_loc[4] = vec3(-1, -5.5, 4.9);
		tree_loc[5] = vec3(-5, -5.5, 9);
		tree_loc[6] = vec3(-6, -5.5, 2);


		//populate them!!
		for (int j = 0; j < 7; j++) {
			SetModel(prog, tree_loc[j] + vec3(0, 4.1, 0), 0, 0, 0, 0.15);
			for (int i = 0; i < 12; i++) {
				if (i == 0) {
					glUniform3f(prog->getUniform("MatAmb"), 0, 0, 0);       //trunk
					glUniform3f(prog->getUniform("MatDif"), 0.897093, 0.588047, 0.331905);
					glUniform3f(prog->getUniform("MatSpec"), 0.5, 0.5, 0.5);
					glUniform1f(prog->getUniform("MatShine"), 200);
				}
				else {
					glUniform3f(prog->getUniform("MatAmb"), 0.1, 0.2, 0.1);       //leaves
					glUniform3f(prog->getUniform("MatDif"), 0.285989, 0.567238, 0.019148);
					glUniform3f(prog->getUniform("MatSpec"), 0.5, 0.5, 0.5);
					glUniform1f(prog->getUniform("MatShine"), 200);
					
				}
				tree1[i]->draw(prog);
			}
		}

		prog->unbind();



		//using texture shader now
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));

		SetView(texProg);



		glUniform1i(texProg->getUniform("flip"), 1);
		glUniform3f(texProg->getUniform("MatAmb"), 0.17, 0.05, 0.05);
		glUniform3f(texProg->getUniform("MatSpec"), 0.2, 0.1, 0.1);
		glUniform1f(texProg->getUniform("MatShine"), 5);
		texture3->bind(texProg->getUniform("Texture0"));




		//hierarchical modeling with cat!!
		Model->pushMatrix();
			Model->translate(player_pos + vec3(0, cos(oscillate) * 0.009, 0));
			Model->rotate(player_rot, vec3(0, 1, 0));
			Model->scale(vec3(0.7, 0.7, 0.7));

			
			Model->pushMatrix();  // upper left leg
				Model->translate(vec3(0.16, 0.48, 0.35));
				Model->rotate(f[cur_idx][0] * (1 - frame) + f[next_idx][0] * frame, vec3(1, 0, 0));
				Model->translate(vec3(0, -0.09, 0));

				Model->pushMatrix();  // upper left calf
					Model->translate(vec3(0, -0.17, 0));
					Model->rotate(f[cur_idx][1] * (1 - frame) + f[next_idx][1] * frame, vec3(1, 0, 0));
					Model->translate(vec3(0, -0.17, 0));

					Model->pushMatrix();  // upper left foot
						Model->translate(vec3(0, -0.15, -0.018));
						Model->rotate(f[cur_idx][2] * (1 - frame) + f[next_idx][2] * frame, vec3(1, 0, 0));
						Model->translate(vec3(0, 0, 0.09));

						Model->scale(vec3(0.08, 0.045, 0.1));
						setModel(texProg, Model);
						sphere->draw(texProg);
					Model->popMatrix();

					Model->scale(vec3(0.08, 0.18, 0.084));
					setModel(texProg, Model);
					sphere->draw(texProg);
				Model->popMatrix();

				Model->scale(vec3(0.11, 0.29, 0.12));
				setModel(texProg, Model);
				sphere->draw(texProg);
			Model->popMatrix();




			Model->pushMatrix(); // upper right leg
				Model->translate(vec3(-0.16, 0.48, 0.35));
				Model->rotate(f[cur_idx][3] * (1 - frame) + f[next_idx][3] * frame, vec3(1, 0, 0));
				Model->translate(vec3(0, -0.09, 0));

				Model->pushMatrix();  // upper right calf
					Model->translate(vec3(0, -0.17, 0));
					Model->rotate(f[cur_idx][4] * (1 - frame) + f[next_idx][4] * frame, vec3(1, 0, 0));
					Model->translate(vec3(0, -0.17, 0));

					Model->pushMatrix();  // upper right foot
						Model->translate(vec3(0, -0.15, -0.018));
						Model->rotate(f[cur_idx][5] * (1 - frame) + f[next_idx][5] * frame, vec3(1, 0, 0));
						Model->translate(vec3(0, 0, 0.09));

						Model->scale(vec3(0.08, 0.045, 0.1));
						setModel(texProg, Model);
						sphere->draw(texProg);
					Model->popMatrix();

					Model->scale(vec3(0.08, 0.18, 0.084));
					setModel(texProg, Model);
					sphere->draw(texProg);
				Model->popMatrix();

				Model->scale(vec3(0.11, 0.29, 0.12));
				setModel(texProg, Model);
				sphere->draw(texProg);
			Model->popMatrix();




			Model->pushMatrix(); // lower left leg
				Model->translate(vec3(0.2, 0.58, -0.29));
				Model->rotate(f[cur_idx][6] * (1 - frame) + f[next_idx][6] * frame, vec3(1, 0, 0));
				Model->translate(vec3(0, -0.27, 0));

				Model->pushMatrix();  // lower left calf
					Model->translate(vec3(0, -0.18, 0));
					Model->rotate(f[cur_idx][7] * (1 - frame) + f[next_idx][7] * frame, vec3(1, 0, 0));
					Model->translate(vec3(0, -0.15, 0));

					Model->pushMatrix();  // lower left foot
						Model->translate(vec3(0, -0.13, 0));
						Model->rotate(f[cur_idx][8] * (1 - frame) + f[next_idx][8] * frame, vec3(1, 0, 0));
						Model->translate(vec3(0, 0, 0.045));

						Model->scale(vec3(0.08, 0.05, 0.11));
						setModel(texProg, Model);
						sphere->draw(texProg);
					Model->popMatrix();

					Model->scale(vec3(0.082, 0.17, 0.082));
					setModel(texProg, Model);
					sphere->draw(texProg);
				Model->popMatrix();

				Model->scale(vec3(0.12, 0.3, 0.12));
				setModel(texProg, Model);
				sphere->draw(texProg);
			Model->popMatrix();





			Model->pushMatrix(); // lower right leg
				Model->translate(vec3(-0.2, 0.58, -0.29));
				Model->rotate(f[cur_idx][9] * (1 - frame) + f[next_idx][9] * frame, vec3(1, 0, 0));
				Model->translate(vec3(0, -0.27, 0));

				Model->pushMatrix();  // lower right calf
					Model->translate(vec3(0, -0.18, 0));
					Model->rotate(f[cur_idx][10] * (1 - frame) + f[next_idx][10] * frame, vec3(1, 0, 0));
					Model->translate(vec3(0, -0.15, 0));

					Model->pushMatrix();  // lower right foot
						Model->translate(vec3(0, -0.13, 0));
						Model->rotate(f[cur_idx][11] * (1 - frame) + f[next_idx][11] * frame, vec3(1, 0, 0));
						Model->translate(vec3(0, 0, 0.045));

						Model->scale(vec3(0.08, 0.05, 0.11));
						setModel(texProg, Model);
						sphere->draw(texProg);
					Model->popMatrix();

					Model->scale(vec3(0.082, 0.17, 0.082));
					setModel(texProg, Model);
					sphere->draw(texProg);
				Model->popMatrix();



				Model->scale(vec3(0.12, 0.3, 0.12));
				setModel(texProg, Model);
				sphere->draw(texProg);
			Model->popMatrix();


			Model->pushMatrix(); // tail base
				Model->translate(vec3(0, 0.62, -0.27));
				Model->rotate(2.1, vec3(1, 0, 0));
				Model->translate(vec3(0, -0.27, 0));

				Model->pushMatrix();  // main stiff part
					Model->translate(vec3(0, -0.12, 0));
					Model->rotate(0.8, vec3(1, 0, 0));
					Model->translate(vec3(0, -0.15, 0));

					Model->pushMatrix();  // wiggling part
						Model->translate(vec3(0, -0.13, 0));
						Model->rotate(cos(glfwGetTime()*3) / 2, vec3(0, 0, 1));
						Model->translate(vec3(0, -0.13, 0));

						Model->pushMatrix();  // wiggling tip
							Model->translate(vec3(0, -0.06, 0));
							Model->rotate(cos(glfwGetTime()*5) / 3, vec3(0, 0, 1));
							Model->translate(vec3(0, -0.13, 0));

							Model->scale(vec3(0.09, 0.16, 0.09));
							setModel(texProg, Model);
							sphere->draw(texProg);
						Model->popMatrix();


						Model->scale(vec3(0.1, 0.16, 0.1));
						setModel(texProg, Model);
						sphere->draw(texProg);
					Model->popMatrix();

					Model->scale(vec3(0.11, 0.22, 0.11));
					setModel(texProg, Model);
					sphere->draw(texProg);
				Model->popMatrix();

				Model->scale(vec3(0.12, 0.2, 0.12));
				setModel(texProg, Model);
				sphere->draw(texProg);
			Model->popMatrix();

			texture3->unbind();
			texture2->bind(texProg->getUniform("Texture0"));


			
			glUniform3f(texProg->getUniform("MatAmb"), 0.1, 0.05, 0.05);
			Model->scale(vec3(0.4, 0.4, 0.4));
			setModel(texProg, Model);
			cat->draw(texProg); // body !


		Model->popMatrix();

		texture2->unbind();


		//sky box!
		glUniform1i(texProg->getUniform("flip"), 0);
		
		glUniform3f(texProg->getUniform("MatAmb"), 0.2, 0.3, 0.65);
		
		glUniform1f(texProg->getUniform("MatShine"), 100.0);
		texture1->bind(texProg->getUniform("Texture0"));
		Model->pushMatrix();
			Model->loadIdentity();
			Model->scale(vec3(20.0));
			setModel(texProg, Model);
			sphere->draw(texProg);
		Model->popMatrix();

		texProg->unbind();


		drawGround(texProg);  //draw ground here


		//halt animations if cat collides with flower or tree
		check_collision(flower_loc, 7, tree_loc, 7, player_pos);
		

		//update animation variables
		sTheta = -1*abs(sin(glfwGetTime() * 2));
		cTheta = cos(glfwGetTime()) / 4;
		if (animate) {
			frame += 0.1;
			if (frame >= 0.99 && frame <= 1.01) {
				frame = 0.0;
				cur_idx = (cur_idx + 1) % 5;
				next_idx = (next_idx + 1) % 5;
			}
			oscillate += 0.2;  //oscillate while walking
		}		
		oscillate += 0.02; //to make sure cat is not entirely stagnant

		bounds = std::sqrt(   //update cat's distance from skybox
			player_pos[0] * player_pos[0]
			+ player_pos[2] * player_pos[2]
		);

		// Pop matrix stacks.
		Projection->popMatrix();

		bf.updateMotion(frametime);

	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	
	application->initGeom(resourceDir);

	auto lastTime = chrono::high_resolution_clock::now();

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// save current time for next frame
		auto nextLastTime = chrono::high_resolution_clock::now();

		// get time since last frame
		float deltaTime =
			chrono::duration_cast<std::chrono::microseconds>(
				chrono::high_resolution_clock::now() - lastTime)
				.count();
		// convert microseconds (weird) to seconds (less weird)
		deltaTime *= 0.000001;

		// reset lastTime so that we can calculate the deltaTime
		// on the next frame
		lastTime = nextLastTime;

		// Render scene.
		application->render(deltaTime);

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}