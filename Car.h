#ifndef __CAR__
#define __CAR__

#include "GameObject.h"

const int CARRADIUS = 4;

class Car : public GameObject {
	private:
		float _maxSpeed = 0.02f;
		float _dirX = 1;
		float _dirZ = 0;
		int _lifes = 5;
		int _score = 0;

	public:
		Car(int lives);
		float getDirX();
		float getDirZ();
		int getLifes();
		void discountLife();
		void setLifes(int lifes);
		void treatCollision(GameObject * obj);
		void update(double timePassed);
		void resetCar();
		void increaseScore();
		int getScore();
		void setScore(int score);
};

#endif