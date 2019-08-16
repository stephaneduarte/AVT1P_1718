#include "vec3.h"

vec3::vec3() {
	_x = 0.0;
	_y = 0.0;
	_z = 0.0;
}

vec3::vec3(float x, float y, float z) {
	_x = x;
	_y = y;
	_z = z;
}

float vec3::getX() const {
	return _x;
}

float vec3::getY() const {
	return _y;
}

float vec3::getZ() const {
	return _z;
}

void vec3::setX(float x) {
	_x = x;
}

void vec3::setY(float y) {
	_y = y;
}

void vec3::setZ(float z) {
	_z = z;
}

void vec3::set(float x, float y, float z) {
	_x = x;
	_y = y;
	_z = z;
}