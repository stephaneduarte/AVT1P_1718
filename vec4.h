#ifndef __vec4__
#define __vec4__

#include "vec3.h"

class vec4 : public vec3 {
public:
	vec4();
	vec4(float in_x, float in_y, float in_z, float in_w);
	float getW() const;
	void set(float in_x, float in_y, float in_z, float in_w);
private:
	float _w;
};
#endif