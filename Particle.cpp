#include "Particle.h"
#include <iostream>


Particle::Particle(float life, float fade, float red, float green, float blue, float x, float y, float z, float vx, float vy, float vz, float ax, float ay, float az) {
	_life = life;
	_fade = fade;
	_red = red;
	_green = green;
	_blue = blue;
	_x = x;
	_y = y;
	_z = z;
	_vx = vx;
	_vy = vy;
	_vz = vz;
	_ax = ax;
	_ay = ay;
	_az = az;
}


void Particle::update(double timePassed) {
	
	this->setX(this->getX() + (this->getVx()* timePassed));
	this->setY(this->getY() + (this->getVy()* timePassed));
	this->setZ(this->getZ() + (this->getVz()* timePassed));

	this->setVx(this->getVx() + (this->getAx()* timePassed));
	this->setVy(this->getVy() + (this->getAy()* timePassed));
	this->setVz(this->getVz() + (this->getAz()* timePassed));

	_life -= _fade;

	if (this->getY() < 0 ) {
		_life = 0;
	}

}

float Particle::getLife() {
	return _life;
}

float Particle::getX() { return _x; }

float Particle::getY() { return _y; }

float Particle::getZ() { return _z; }

float Particle::getVx() { return _vx; }

float Particle::getVy() { return _vy; }

float Particle::getVz() { return _vz; }

float Particle::getAx() { return _ax; }

float Particle::getAy() { return _ay; }

float Particle::getAz() { return _az; }

float Particle::getRed() { return _red; }

float Particle::getGreen() { return _green; }

float Particle::getBlue() { return _blue; }

void Particle::setX(float x) { _x = x; }

void Particle::setY(float y) { _y = y; }

void Particle::setZ(float z) { _z = z; }

void Particle::setVx(float vx) { _vx = vx; }

void Particle::setVy(float vy) { _vy = vy; }

void Particle::setVz(float vz) { _vz = vz; }
