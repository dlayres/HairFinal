#ifndef _SPRING_H_
#define _SPRING_H_

#include "Vector.h"
#include "Particle.h"

class Spring {
private:
	int i, j;	//Indicies of connecting particles
	double k;	//Spring constand
	double l;	//Spring length

public:
	Spring();
	Spring(int, int, double, double);

	//Draw line segments between particles
	void render(std::vector<Particle>);

	//Getters/setters
	int getFirst();
	int getSecond();
	double getConstant();
	double getLength();
	void setFirst(int);
	void setSecond(int);
	void setConstant(double);
	void setLength(double);
};

#endif
