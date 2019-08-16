#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <string>

const float _aceleration = 0.000005;
const float _aceleration2 = 5 * _aceleration;
const float _aceleration3 = 8 * _aceleration;
#define TABLELIMITE 100
#define PI 3.14159265


class GameObject {
	private:
		/*float _speedx;
		float _speedz;

		float _acelerationx;
		float _acelerationz;*/
		
		float _auxPosX;
		float _auxPosY;
		float _auxPosZ;

		float _speed;
		float _faceleration;

		float _posX;
		float _posY;
		float _posZ;

		/*float _dirX;
		float _dirY;
		float _dirZ;*/

		float _rotSpeed;
		double _rotY;

		float _radius;
		std::string _name;

		bool _dead;

	public:
		GameObject();
		GameObject(float x, float y, float z, float speed, float radius, std::string name);
		virtual void update(double timePassed);
		void newPosition();
		float getPositionX();
		float getPositionY();
		float getPositionZ();
		float getAuxPositionX();
		float getAuxPositionZ();
		void setAuxPosition(float x, float y, float z);
		virtual void setPosition();
		/*void setAcelerationX(float aceleration);
		void setAcelerationZ(float aceleration);*/
		void setAceleration(float aceleration);
		float getAceleration();
		/*void setAuxPosX(float auxPosX);
		void setAuxPosZ(float auxPosZ);
		float getAcelerationX();
		float getAcelerationZ();
		float getSpeedX();
		float getSpeedZ();
		void setSpeedX(float speed);*/
		float getSpeed();
		void setSpeed(float speed);
		void setRotationSpeed(float speed);
		void increaseRotation(double rotY);
		void increasePosition(float x, float y, float z);
		float getRotationSpeed();
		float getRotY();
		void setRotY(float rotY);
		float getRadius();
		void setRadius(float radius);
		bool hasCollision(GameObject* obj);
		virtual void treatCollision(GameObject* obj);
		std::string getName();
		void setName(std::string name);
		virtual bool isNew();
		void setDead();
		bool isDead();
};

#endif