#include "Orange.h"
#include "GameObject.h"
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <ctime>

Orange::Orange(float posX, float posY, float posZ) : GameObject(posX, posY, posZ, 0.0f, ORANGERADIUS, "Orange") {
	_new = 1;
	_angle = ((float)std::rand() / (float)RAND_MAX) * 360;
	_speedFactor = 0.01f;
	this->setSpeed((float)std::rand() / (float)RAND_MAX * _speedFactor + 0.005f);
}

void Orange::update(double timePassed) {
	_time += timePassed;
	if (this->getName() == "Orange") {
		if (_time > 10000) {
			this->setSpeedFactor(_speedFactor + 0.005f);
			_time = 0;
		}
		float distance = this->getSpeed() * timePassed;
		float dx = distance * cos(_angle);
		float dz = distance * sin(_angle);
		this->increasePosition(dx, 0, dz);
		if (this->getPositionX() > TABLELIMITE || this->getPositionX() < 0 || this->getPositionZ() > TABLELIMITE || this->getPositionZ() < 0) newPosition();
	}
	
}

void Orange::newPosition() {
	_new = 1;
	//Aparecem random mas apenas dentro do quadrado do meio
	this->setAuxPosition((float)std::rand() / (float)RAND_MAX * 40 + 30.0f, this->getPositionY(), (float)std::rand() / (float)RAND_MAX * 40 + 30.0f);
	_angle = ((float)std::rand() / (float)RAND_MAX) * 360;
	this->setSpeed((float)std::rand() / (float)RAND_MAX * _speedFactor + 0.005f);
	//printf("My new speed is: %f.\n", this->getSpeed());
}

void Orange::treatCollision(GameObject* obj) {
	if (!_new) {
		if (obj->getName() == "Orange" || obj->getName() == "Car") {
			//O new evita que duas laranjas sejam criadas sobrepostas
			//volta pra tras
			this->setSpeed(-this->getSpeed());
			//retorna a ultima posicao
			this->setAuxPosition(this->getPositionX(), this->getPositionY(), this->getPositionZ());
		}
	}
	else this->newPosition();
}

void Orange::setPosition() {
	if (_new = 1) _new = 0;
	GameObject::setPosition();
}

bool Orange::isNew() {
	return _new;
}

void Orange::setSpeedFactor(float speedFactor) {
	_speedFactor = speedFactor;
}
