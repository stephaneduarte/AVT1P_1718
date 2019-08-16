#ifndef __ORANGE__
#define __ORANGE__

#include "GameObject.h"

const int ORANGERADIUS = 2;

class Orange : public GameObject {
private:
	float _angle;
	float _speedFactor;
	double _time = 0;
	bool _new = 0;

public:
	Orange(float posX, float posY, float posZ);
	virtual void update(double timePassed) override;
	void newPosition();
	void treatCollision(GameObject * obj);
	void setPosition();
	bool isNew();
	void setSpeedFactor(float speedFactor);
};

#endif