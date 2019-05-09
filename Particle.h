#ifndef _PARTICLE_H_
#define _PARTICLE_H_

 #include <vector>
 #include "Vector.h"
 
 class Particle {
 private:
   bool   stationary;	//Tracks if its a user controlled particle
   Vector pos;
   Vector velocity;
   Vector color;
   Vector force;
   double mass;

 public:
	Particle();                                  
	Particle(Vector, double);           

	void render();	//Draws itself as a dot
	
	void toggleMovement();	//Changes stationary setting

	bool   isStationary();
 	Vector getPosition();
	Vector getVelocity();
	Vector getForce();
 	double getMass();

	void setStationary(bool);
 	void setPosition(Vector);
 	void setPosition(double, double, double);
	void setVelocity(Vector);
	void setVelocity(double, double, double);
	void setColor(Vector);
	void setColor(double, double, double);
	void setForce(Vector);
	void setForce(double, double, double);
 	void setMass(double);

 };

#endif
