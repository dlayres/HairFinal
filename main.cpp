/*
 * main.cpp
 * 
 * Written by: Sean Brown
 * 
 * Particle:
 * x, y, z - vectors of 3 (position
 * m - real number (mass)
 * 
 * Spring:
 * index i, j - integer (particles)
 * k - real number (spring constant)
 * l - real number (distance of rest state)
 * 
 * Force on particle p
 *   Fp = sum of forces connected to p
 * 
 * Force between particles
 *   Fp-pi = -spring.k(||p - pi|| - spring.l)(p - pi)/(||p - pi||)
 * 
 */
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

// flags
bool debug = false;
bool gravity;
bool simulation;

// settings
double nearPlane  = 1.0f;
double farPlane   = 1000.0f;
double fov        = 60.0f;
double damp       = 0.1;
double timeStep   = 0.000000001f;
int counter = 0;

int pointsPerStrand = 3;

// glfw info
int width       = 1024;
int height      = 768;
string windowName;

Vector camera;
vector<Particle> particles;
vector<Spring> springs;

void getGravity() {
  char input[256];

  file >> input;
  if(!string(input).compare(string("false"))) {
    gravity = false;
  }
  else {
    gravity = true;
  }
}

void getParticles(int num) {
  double x, y, z, w;

  for(int i = 0; i < num; i++) {
    file >> x;
    file >> y;
    file >> z;
    file >> w;
    particles.push_back(Particle(Vector(x, y, z), w));
  }
}

void getSprings(int num) {
  int i, j;
  double k, l;

  file.get();

  for(int a = 0; a < num; a++) {
    file >> i;
    file >> j;
    file >> k;
    file >> l;
    springs.push_back(Spring(i, j, k, l));
  }
}

void readFile(string filename) {
  char title[256];
  char mode;
  int  amount;
  int  stillParticle;

  file.open(filename.c_str());

  if(!file.is_open()) {
    cout << "File couldn't be open" << endl;
    exit(EXIT_FAILURE);
  }

  file.getline(title, 256);
  windowName = string(title);

  // get gravity setting
  file >> mode;
  if(mode == 'g') {
    getGravity();
  }
  else {
    cout << "No gravity setting" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "getting particles" << endl;
  // get particles
  file >> mode;
  if(mode == 'p') {
    file >> amount;
    file >> stillParticle;
    getParticles(amount);
    /*if(stillParticle >= 0) {
      particles[stillParticle].toggleMovement();
	  particles[3].toggleMovement();
	  particles[6].toggleMovement();
	  particles[9].toggleMovement();
    }*/

		particles[0].toggleMovement();


  }
  else {
    cout << "improper particle section" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "getting springs" << endl;
  // get springs
  file >> mode;
  if(mode == 's') {
    file >> amount;
    getSprings(amount);
  }
  else {
    cout << "improper spring section" << endl;
    exit(EXIT_FAILURE);
  }

  file.close();
  cout << "finished loading" << endl;
}

void init() {
  simulation = false;

  camera = Vector(0.0f, 0.0f, 0.0f);

  glPointSize(10.0f);
}

void update() {
  Vector v1, v2, v3; // for dampening
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

    // check for length of 0
    springVector.normalize();

    // calculate force
	  force = ((springVector * hookesValue));

	  p1->setForce(p1->getForce() + force);
	  p2->setForce(p2->getForce() - force);

    if(debug) {
      cout << "gravity = " << gravity << endl;
      cout << "v1 = ";
      p1->getVelocity().print();
      v1.print();
      cout << "v2 = ";
      p2->getVelocity().print();
      v2.print();
      cout << "v3 = ";
      v3.print();
      cout << "hooke value = " << hookesValue << endl;
      cout << "dampening force = ";
      dampeningForce.print();
      cout << "springVector = ";
      springVector.print();
      cout << "Spring length = " << springLength << endl;
      cout << "force = ";
      force.print();
    }
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

/* my update
void update() {


	for (unsigned int i = 0; i < particles.size(); i++) {
		if (!particles[i].isStationary()) {
			//	if (gravity) {
				//	particles[i].setForce(particles[i].getForce() + Vector(0.0f, -9.81f, 0.0f) * particles[i].getMass());
				//}

			Vector posChange = particles[i].getForce() / 5.0f;

			cout << posChange.getX() << " " << posChange.getY() << " " << posChange.getZ() << endl;

			particles[i].setPosition(particles[i].getPosition() + posChange);

		}
	}

	for (unsigned int i = 0; i < springs.size(); i++) {
		p1 = &particles[springs[i].getFirst()];
		p2 = &particles[springs[i].getSecond()];

		springVector = p1->getPosition() - p2->getPosition();
		springLength = springVector.length();
		distanceFromRest = (springLength - springs[i].getLength());

		hookesValue = -springs[i].getConstant() * distanceFromRest;

		// check for length of 0
		springVector.normalize();

		// calculate force
		force = ((springVector * hookesValue));

		p1->setForce(p1->getForce() + force);
		p2->setForce(p2->getForce() - force);
	}
}
*/

void glhFrustumf2(float *matrix, float left, float right, float bottom, float top, float znear, float zfar) {
	float temp, temp2, temp3, temp4;
	temp = 2.0 * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = temp / temp2;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = temp / temp3;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = (right + left) / temp2;
	matrix[9] = (top + bottom) / temp3;
	matrix[10] = (-zfar - znear) / temp4;
	matrix[11] = -1.0;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = (-temp * zfar) / temp4;
	matrix[15] = 0.0;
}

void glhPerspectivef2(float *matrix, float fovyInDegrees, float aspectRatio,
	float znear, float zfar)
{
	float ymax, xmax;
	ymax = znear * tanf(fovyInDegrees * 3.1415 / 360.0);
	//ymin = -ymax;
	//xmin = -ymax * aspectRatio;
	xmax = ymax * aspectRatio;
	glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void render() {
	//CSCI441::drawSolidCube(5.0f);
	
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float matrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	
	//gluPerspective(fov, height/width, nearPlane, farPlane);
	//glhPerspectivef2(matrix, fov, height / width, nearPlane, farPlane);
	//glLoadMatrixf(matrix);
  //gluLookAt(camera.getX(), camera.getY(), camera.getZ(), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();
  glTranslatef(camera.getX(), camera.getY(), camera.getZ());

  for(unsigned int i = 0; i < springs.size(); i++) {
  	springs[i].render(particles);
  }

  glPopMatrix();
  glPushMatrix();

  glTranslatef(5, 5, 5);
  
  glColor3f(1.0f, 0.0f, 0.0f);
  GLUquadric *quad;
  quad = gluNewQuadric();
  gluDisk(quad, 500000.0f, 500000.0f, 20, 20);

  glPopMatrix();

  glfwSwapBuffers(window);
  glfwPollEvents();
}

void keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS || action == GLFW_REPEAT) {
    if(key == GLFW_KEY_ENTER) {
      update();
    }
    if(key == GLFW_KEY_SPACE) {
      simulation = !simulation;
    }
    if(key == GLFW_KEY_D) {
      debug = !debug;
    }
    if(key == GLFW_KEY_G) {
      gravity = !gravity;
    }
    if(key == GLFW_KEY_DOWN) {
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
    if(key == GLFW_KEY_UP) {
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
    if(key == GLFW_KEY_LEFT) {
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
    if(key == GLFW_KEY_RIGHT) {
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
    }
	if (key == GLFW_KEY_W) {
		for (unsigned int i = 0; i < particles.size(); i += pointsPerStrand) {
			Vector currPos = particles[i].getPosition();
			Vector newPos = Vector(currPos.getX(), currPos.getY(), currPos.getZ() + 0.04);
			particles[i].setPosition(newPos);
		}
		// camera.setX(camera.getX() - 10.0f);
	}

  }

 // update();
}

int main(int argc, char **argv)
{
	
  if(argc != 2) {
    cout << "Need a file." << endl;
    exit(EXIT_FAILURE);
  }

  readFile(string(argv[1]));

	if(!glfwInit()) {
    cout << "glfw failed to initialize" << endl;
		exit(EXIT_FAILURE);
	}
	
	window = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);
	if(!window) {
    cout << "Window failed to be created" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
  
	glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyboardFunc);

	init();
  
	while(!glfwWindowShouldClose(window)) {
		counter++;
    if(simulation && (counter % 10 == 0)) {
      update();
    }

    render();
  }
  
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}

