#ifndef __BUTTER__
#define __BUTTER__

#include "GameObject.h"

const int BUTTERRADIUS = 1;

class Butter : public GameObject {

public:
	Butter(float x, float y, float z);
	virtual void update(double timePassed) override;
	void treatCollision(GameObject * obj);
	void deleteButter();
};
#endif
