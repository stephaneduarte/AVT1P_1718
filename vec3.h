#ifndef __VEC3__
#define __VEC3__

#include <stdio.h>
#include <cmath>

class vec3 {
public:
	vec3();
	vec3(float x, float y, float z);
	float getX() const;
	float getY() const;
	float getZ() const;
	void setX(float x);
	void setY(float x);
	void setZ(float x);
	void set(float x, float y, float z);
private:
	float _x;
	float _y;
	float _z;
};

#endif