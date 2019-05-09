#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GL/glew.h> // include GLEW to get our OpenGL 3.0+ bindings
#include <GLFW/glfw3.h>

#include "Spring.h"
#include "Particle.h"
#include "Vector.h"
#include <cmath>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/modelLoader3.hpp> // to load in OBJ models
#include <CSCI441/objects3.hpp> // to render our 3D primitives
#include <CSCI441/OpenGLUtils3.hpp> // to print info about OpenGL
#include <CSCI441/ShaderProgram3.hpp> // our shader helper functions
#include <CSCI441/TextureUtils.hpp> // our texture helper functions


using namespace std;

GLFWwindow* window;
ifstream file;

bool debug = false;
bool gravity;
bool simulation;

double nearPlane  = 1.0f;
double farPlane   = 1000.0f;
double fov        = 60.0f;
double damp       = 0.1;
double timeStep   = 0.0000001f;
int counter = 0;

int pointsPerStrand = 5;

int width       = 1024;
int height      = 768;
string windowName;

Vector camera;
vector<Particle> particles;
vector<Spring> springs;

void setup() {
	simulation = false;

	camera = Vector(0.0f, 0.0f, 0.0f);

	glPointSize(5.0f);
}

void loadPoints() {
	int num_strands = 12;
	int num_particles = 5;

	double strand_spacing = 0.02;
	double particle_weight = 4.0;

	double spring_stiffness = 0.05;

	double particle_spacing = 0.002;
	double spring_length = 0.0000000002;
	double x = 0;
	double y = 0.8;

	for (int i = 0; i < num_strands; i++) {
		y = 0.8;
		spring_stiffness -= (num_particles/2)*.001;
		for (int j = 0; j < num_particles; j++) {
			if (j == 0) {
				Particle p = Particle(Vector(x, y, -1.0), particle_weight);
				p.toggleMovement();
				particles.push_back(p);
			} else {
				particles.push_back(Particle(Vector(x, y, -1.0), particle_weight));
			}
			if (j != (num_particles - 1)) {
				springs.push_back(Spring(i*num_particles + j, i*num_particles + j + 1, spring_stiffness, spring_length));
				spring_stiffness += .001;
			}
			y -= particle_spacing;
		}
		x += strand_spacing;
	}

}

void keyboardCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods) {
	bool downPressed = false;
	bool upPressed = false;
	bool leftPressed = false;
	bool rightPressed = false;

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_ENTER) {
			update();
		}
		if (key == GLFW_KEY_SPACE) {
			simulation = !simulation;
		}
		if (key == GLFW_KEY_D) {
			debug = !debug;
		}
		if (key == GLFW_KEY_G) {
			gravity = !gravity;
		}
		if (key == GLFW_KEY_DOWN) {
			downPressed = true;
			upPressed = false;
		}
		else if (key == GLFW_KEY_UP) {
			upPressed = true;
			downPressed = false;
		}
		if (key == GLFW_KEY_LEFT) {
			leftPressed = true;
			rightPressed = false;
		}
		else if (key == GLFW_KEY_RIGHT) {
			rightPressed = true;
			leftPressed = false;
		}
	}
}

void update() {
  Vector v1, v2, v3; 
  Particle *p1, *p2;
  Vector forceOverLength;
  Vector force;
  Vector springVector;
  double springLength;
  double distanceFromRest;
  double hookesValue;
  Vector dampeningForce;

  for(unsigned int i = 0; i < springs.size(); i++) {
	  p1 = &particles[springs[i].getFirst()];
	  p2 = &particles[springs[i].getSecond()];

	  springVector = p1->getPosition() - p2->getPosition();
	  springLength = springVector.length();
	  distanceFromRest = (springLength - springs[i].getLength());
	  
	  hookesValue = -springs[i].getConstant() * distanceFromRest;
	  
	  springVector.normalize();

	  force = ((springVector * hookesValue));

	  p2->setForce(p2->getForce() - force);
	}

	for(unsigned int i = 0; i < particles.size(); i++) {
		if(!particles[i].isStationary()) {
			if(gravity) {
				particles[i].setForce(particles[i].getForce() + Vector(0.0f, -0.0081f, 0.0f) * particles[i].getMass());
			}
			particles[i].setVelocity(particles[i].getVelocity() + (particles[i].getForce() / (particles[i].getMass() * timeStep)));
			particles[i].setVelocity(particles[i].getVelocity() * damp);
			particles[i].setPosition(particles[i].getPosition() + (particles[i].getVelocity() * timeStep));
			particles[i].setForce(Vector(0.0f, 0.0f, 0.0f));
		}
	}
}

void render() {
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(camera.getX(), camera.getY(), camera.getZ());

	for(unsigned int i = 0; i < springs.size(); i++) {
  		springs[i].render(particles);
	}

	glPopMatrix();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main(int argc, char **argv){
	
	setup();
	loadPoints();
  
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyboardCallbacks);
  
	while(!glfwWindowShouldClose(window)) {
		if(simulation) {
			update();
		}
		render();

		//keyboard callbacks get called more often
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			for (unsigned int i = 0; i < particles.size(); i++) {
				if (i % pointsPerStrand == 0) {
					Vector currPos = particles[i].getPosition();
					Vector newPos = Vector(currPos.getX(), currPos.getY() - 0.01, currPos.getZ());
					particles[i].setPosition(newPos);
				}
				else {
					update();
				}
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			for (unsigned int i = 0; i < particles.size(); i++) {
				if (i % pointsPerStrand == 0) {
					Vector currPos = particles[i].getPosition();
					Vector newPos = Vector(currPos.getX(), currPos.getY() + 0.01, currPos.getZ());
					particles[i].setPosition(newPos);
				}
				else {
					update();
				}
			}
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			for (unsigned int i = 0; i < particles.size(); i++) {
				if (i % pointsPerStrand == 0) {
					Vector currPos = particles[i].getPosition();
					Vector newPos = Vector(currPos.getX() - 0.01, currPos.getY(), currPos.getZ());
					particles[i].setPosition(newPos);
				}
				else {
					update();
				}
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			for (unsigned int i = 0; i < particles.size(); i++) {
				if (i % pointsPerStrand == 0) {
					Vector currPos = particles[i].getPosition();
					Vector newPos = Vector(currPos.getX() + 0.01, currPos.getY(), currPos.getZ());
					particles[i].setPosition(newPos);
				}
				else {
					update();
				}
			}
		}
	}
  
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}

/*
if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
for (unsigned int i = 0; i < particles.size(); i++) {
if (i % pointsPerStrand == 0) {
Vector currPos = particles[i].getPosition();
Vector newPos = Vector(currPos.getX(), currPos.getY() - 0.04, currPos.getZ());
particles[i].setPosition(newPos);
}
else {
update();
}
}
}
else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
for (unsigned int i = 0; i < particles.size(); i++) {
if (i % pointsPerStrand == 0) {
Vector currPos = particles[i].getPosition();
Vector newPos = Vector(currPos.getX(), currPos.getY() + 0.04, currPos.getZ());
particles[i].setPosition(newPos);
}
else {
update();
}
}
}
if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
for (unsigned int i = 0; i < particles.size(); i++) {
if (i % pointsPerStrand == 0) {
Vector currPos = particles[i].getPosition();
Vector newPos = Vector(currPos.getX() - 0.04, currPos.getY(), currPos.getZ());
particles[i].setPosition(newPos);
}
else {
update();
}
}
}
else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
for (unsigned int i = 0; i < particles.size(); i++) {
if (i % pointsPerStrand == 0) {
Vector currPos = particles[i].getPosition();
Vector newPos = Vector(currPos.getX() + 0.04, currPos.getY(), currPos.getZ());
particles[i].setPosition(newPos);
}
else {
update();
}
}
}*/

