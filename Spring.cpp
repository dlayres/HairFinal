#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <iostream>

#include "Spring.h"

Spring::Spring() {

}

Spring::Spring(int i, int j, double k, double l) {
	this->i = i;
	this->j = j;
	this->k = k;
	this->l = l;
}

void Spring::render(std::vector<Particle> particles) {
	particles[i].render();
	particles[j].render();

	Vector p_one = particles[i].getPosition();
	Vector p_two = particles[j].getPosition();

	glBegin(GL_LINES);
	glColor3f(0.55f, 0.45f, 0.2f);
	glVertex3f(p_one.getX(), p_one.getY(), p_one.getZ());
	glVertex3f(p_two.getX(), p_two.getY(), p_two.getZ());
	glEnd();
}

int Spring::getFirst() {
	return i;
}

int Spring::getSecond() {
	return j;
}

double Spring::getConstant() {
	return k;
}

double Spring::getLength() {
	return l;
}

void Spring::setFirst(int i) {
	this->i = i;
}

void Spring::setSecond(int j) {
	this->j = j;
}

void Spring::setConstant(double k) {
	this->k = k;
}

void Spring::setLength(double l) {
	this->l = l;
}
