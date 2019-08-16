#include "GameObject.h"
#include <iostream>

GameObject::GameObject(){}

GameObject::GameObject(float x, float y, float z, float speed, float radius, std::string name) {
	/*_speedx = speedx;
	_speedz = speedz;
	_acelerationx = 0;
	_acelerationz = 0;*/

	_speed = speed;
	_faceleration = 0;

	_auxPosX = x;
	_auxPosY = y;
	_auxPosZ = z;

	_posX = x;
	_posY = y;
	_posZ = z;

	_rotY = 0;
	_rotSpeed = 0;

	_radius = radius;
	_name = name;

	_dead = 0;
}

void GameObject::update(double timePassed) {
	/*_speedx = _speedx + _acelerationx * timePassed;
	_speedz = _speedz + _acelerationz * timePassed;
	_auxPosX = _posX + _speedx * timePassed;
	_auxPosZ = _posZ + _speedz * timePassed;
	newPosition();*/
}

void GameObject::setPosition() {
	_posX = _auxPosX;
	_posY = _auxPosY;
	_posZ = _auxPosZ;
}

void GameObject::setAuxPosition(float x, float y, float z) {
	_auxPosX = x;
	_auxPosY = y;
	_auxPosZ = z;
}

void GameObject::newPosition() {
	/*printf("Entrei na do GameObject");
	setPosition();*/
}



float GameObject::getPositionX() {
	return _posX;
}

float GameObject::getPositionY() {
	return _posY;
}

float GameObject::getPositionZ() {
	return _posZ;
}

float GameObject::getAuxPositionX() {
	return _auxPosX;
}

float GameObject::getAuxPositionZ() {
	return _auxPosZ;
}

void GameObject::setAceleration(float aceleration) {
	_faceleration = aceleration;
}

float GameObject::getSpeed() {
	return _speed;
}

float GameObject::getAceleration() {
	return _faceleration;
}

void GameObject::setSpeed(float speed) {
	_speed = speed;
}

void GameObject::setRotationSpeed(float speed) {
	_rotSpeed = speed;
}

void GameObject::increaseRotation(double rotY) {
	if (_speed != 0) {
		_rotY += rotY * PI / 180.0;
	}
}

float GameObject::getRotationSpeed() {
	return _rotSpeed;
}

float GameObject::getRotY() {
	return _rotY;
}

void GameObject::setRotY(float rotY) {
	_rotY = rotY;
}

void GameObject::increasePosition(float x, float y, float z) {
	_auxPosX += x;
	_auxPosY += y;
	_auxPosZ += z;
}

float GameObject::getRadius() {
	return _radius;
}

void GameObject::setRadius(float radius) {
	_radius = radius;
}

bool GameObject::hasCollision(GameObject* obj) {
	//printf("r1: %f || r2: %f \n", this->getRadius(), obj->getRadius());
	//printf("quadrado: %f || 1º: %f || 2: %f \n", pow(this->getRadius() + obj->getRadius(), 2), pow(this->getPositionX() - obj->getPositionX(), 2), pow(this->getPositionZ() - obj->getPositionZ(), 2));
	if (pow(this->getRadius() + obj->getRadius(), 2) >= pow(this->getAuxPositionX() - obj->getAuxPositionX(), 2) + pow(this->getAuxPositionZ() - obj->getAuxPositionZ(), 2)) {
		/*printf("Detetada colisao.\n");
		printf("r1: %f || r2: %f \n", this->getRadius(), obj->getRadius());
		std::cout << "Objeto 1: " + this->getName() << std::endl;
		printf("POSX:%f\nPOSZ:%f\n", this->getPositionX(), this->getPositionZ());
		std::cout << "Objeto 2: " + this->getName() << std::endl;
		printf("POSX:%f\nPOSZ:%f\n\n", obj->getPositionX(), obj->getPositionZ());
		printf("quadrado: %f || 1º: %f || 2: %f \n", pow(this->getRadius() + obj->getRadius(), 2), pow(this->getPositionX() - obj->getPositionX(), 2), pow(this->getPositionZ() - obj->getPositionZ(), 2));*/
		return true;
	}
	return false;
}

void GameObject::treatCollision(GameObject * obj) {}

std::string GameObject::getName() {
	return _name;
}


void GameObject::setName(std::string name) {
	_name = name;
}

bool GameObject::isNew()
{
	return false;
}

bool GameObject::isDead() {
	return _dead;
}

void GameObject::setDead() {
	_dead = 1;
}