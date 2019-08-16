#include "Cheerio.h"
#include "GameObject.h"
#include <iostream>
#include <math.h>

Cheerio::Cheerio(float posX, float posY, float posZ) : GameObject(posX, posY, posZ, 0, CHEERIORADIUS, "Cheerio"){
}

void Cheerio::update(double timePassed){
	if (this->getSpeed() > 0 && this->getAceleration() > 0 || this->getSpeed() < 0 && this->getAceleration() < 0) {
		this->setAceleration(0.0f);
		this->setSpeed(0.0f);
	}
	this->setSpeed(this->getSpeed() + this->getAceleration() * timePassed);
	float distance = this->getSpeed() * timePassed;
	float dx = distance * cos(this->getRotY());
	float dz = distance * sin(this->getRotY());
	this->increasePosition(dx, 0, dz);
	if (this->getPositionX() > TABLELIMITE || this->getPositionX() < 0 || this->getPositionZ() > TABLELIMITE || this->getPositionZ() < 0) {
		deleteCheerio();
	}
}

void Cheerio::treatCollision(GameObject * obj){
	if (obj->getName() == "Car") {
		this->setSpeed(0.12f * obj->getSpeed());
		this->setAceleration(-0.08f * obj->getAceleration());
		this->setRotY(obj->getRotY());
		//retorna a ultima posicao
		this->setAuxPosition(this->getPositionX(), this->getPositionY(), this->getPositionZ());
	}
}

void Cheerio::deleteCheerio() {
	this->setDead();
}
