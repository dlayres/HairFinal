 #ifndef _VECTOR_H_
 #define _VECTOR_H_

 #include <math.h>
 #include <stdio.h>

class Vector {
	
protected:
	double x, y, z;

public:
	Vector();
	Vector(double, double, double);

	//overrides to make things easier
	Vector operator*(double);
	Vector operator+(Vector);
	Vector operator-(Vector);
  	Vector operator/(double);
  	bool   operator==(Vector);

	//getters/setters
	double getX();
	double getY();
	double getZ();
	void setX(double);
	void setY(double);
	void setZ(double);

	//get magnitude & normalize helper functions
	double length();
	void normalize();
};

#endif
