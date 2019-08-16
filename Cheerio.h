#ifndef __CHEERIO__
#define __CHEERIO__

#include "GameObject.h"

const int CHEERIORADIUS = 0.2;

class Cheerio : public GameObject {

public:
	Cheerio(float posX, float posY, float posZ);
	virtual void update(double timePassed) override;
	void treatCollision(GameObject * obj);
	void deleteCheerio();
};

#endif
