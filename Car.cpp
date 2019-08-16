#include "Car.h"
#include "GameObject.h"
#include <iostream>
#include <math.h>

Car::Car(int lifes) : GameObject(15,0.5,15,0, CARRADIUS, "Car") {
	_lifes = lifes;
}

void Car::update(double timePassed) {
	this->setSpeed(this->getSpeed() + this->getAceleration() * timePassed);
	
	if (this->getAceleration() != _aceleration3 && this->getAceleration() != -_aceleration3 && this->getSpeed() > _maxSpeed) this->setSpeed(_maxSpeed);
	float distance = this->getSpeed() * timePassed;
	float dx = distance * cos(this->getRotY());
	float dz = distance * sin(this->getRotY());
	_dirX = cos(this->getRotY());
	_dirZ = sin(this->getRotY());
	this->increasePosition(dx, 0, dz);
	if (this->getPositionX() > TABLELIMITE - 3.5 || this->getPositionX() < -3.5 || this->getPositionZ() > TABLELIMITE - 3.5 || this->getPositionZ() < -3.5) {
		resetCar();
		this->discountLife();
	}
	if (this->getAceleration() == _aceleration2 && this->getSpeed() > 0 ||
		this->getAceleration() == -_aceleration2 && this->getSpeed() < 0) {
		this->setAceleration(0);
		this->setSpeed(0);
	}

	this->increaseScore();
}

void Car::resetCar() {
	this->setAceleration(0);
	this->setSpeed(0);
	this->setRotY(0);
	this->setAuxPosition(15, 0, 15);
}

float Car::getDirX() {
	return _dirX;
}

float Car::getDirZ() {
	return _dirZ;
}

void Car::treatCollision(GameObject* obj) {
	if (obj->getName() == "Orange") {
		if (!obj->isNew()) {
			this->setSpeed(-0.5f * this->getSpeed());
			this->setAuxPosition(this->getPositionX(), this->getPositionY(), this->getPositionZ());
			this->discountLife();
			this->setScore(this->getScore() / 2);
		}
	}
	if (obj->getName() == "Cheerio") {
		this->setSpeed(-0.5f * this->getSpeed());
		this->setAuxPosition(this->getPositionX(), this->getPositionY(), this->getPositionZ());
		this->setScore(this->getScore() * 4/5);
	}

	if (obj->getName() == "Butter") {
		this->setSpeed(-0.5f * this->getSpeed());
		this->setAuxPosition(this->getPositionX(), this->getPositionY(), this->getPositionZ());
		this->setScore(this->getScore() * 3 / 5);
	}

	if (obj->getName() == "FakeOrange") {
		this->setSpeed(-0.5f * this->getSpeed());
		this->setAuxPosition(this->getPositionX(), this->getPositionY(), this->getPositionZ());
	}
}

int Car::getLifes() {
	return _lifes;
}

void Car::discountLife() {
	--_lifes;
}

void Car::setLifes(int lives) {
	_lifes = lives;
}

void Car::increaseScore() {
	_score++;
}

void Car::setScore(int score) {
	_score = score;
}

int Car::getScore() {
	return _score;
}