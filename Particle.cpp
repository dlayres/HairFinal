#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "Particle.h"


Particle::Particle() {
  this->stationary = false;
	this->pos = Vector(0.0f, 0.0f, 0.0f);
	this->velocity = Vector(0.0f, 0.0f, 0.0f);
	this->color = Vector(0.55f, 0.45f, 0.2f);
	this->force = Vector(0.0f, 0.0f, 0.0f);
	this->mass = 1.0f;
}
  
Particle::Particle(Vector p, double m) {
  this->stationary = false;
	this->pos = p;
	this->velocity = Vector(0.0f, 0.0f, 0.0f);
	this->color = Vector(0.55f, 0.45f, 0.2f);
	this->force = Vector(0.0f, 0.0f, 0.0f);
	this->mass = m;
}

void Particle::render() {
	glBegin(GL_POINTS);
	glColor3f(color.getX(), color.getY(), color.getZ());
	glVertex3f(pos.getX(), pos.getY(), pos.getZ());
	glEnd();
}

void Particle::toggleMovement() {
  this->stationary = !stationary;
}

bool Particle::isStationary() {
  return this->stationary;
}

Vector Particle::getPosition() {
	return this->pos;
}

Vector Particle::getVelocity() {
	return this->velocity;
}

Vector Particle::getForce() {
	return this->force;
}

double Particle::getMass() {
	return this->mass;
}

void Particle::setStationary(bool s) {
  this->stationary = s;
}

void Particle::setPosition(Vector p) {
	this->pos = p;
}

void Particle::setPosition(double x, double y, double z) {
	this->pos = Vector(x, y, z);
}

void Particle::setVelocity(Vector v) {
	this->velocity = v;
}

void Particle::setVelocity(double x, double y, double z) {
	this->velocity = Vector(x, y, z);
}

void Particle::setColor(Vector c) {
	this->color = Vector(0.55f, 0.45f, 0.2f);
}

void Particle::setColor(double r, double g, double b) {
	this->color = Vector(0.55f, 0.45f, 0.2f);
}

void Particle::setForce(Vector f) {
	this->force = f;
}

void Particle::setForce(double x, double y, double z) {
	this->force = Vector(x, y, z);
}

void Particle::setMass(double m) {
	this->mass = m;
}
