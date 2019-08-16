#include "vec4.h"

vec4::vec4() : vec3() {
	_w = 0.0;
}

vec4::vec4(float in_x, float in_y, float in_z, float in_w) : vec3(in_x, in_y, in_z) {
	_w = in_w;
}

void vec4::set(float in_x, float in_y, float in_z, float in_w) {
	vec3::set(in_x, in_y, in_z);
	_w = in_w;
}

float vec4::getW() const{
	return _w;
}