#ifndef __PARTICLE__
#define __PARTICLE__

class Particle {
private:
	float _life;
	float _fade;
	float _red, _green, _blue;
	float _x, _y, _z;
	float _vx, _vy, _vz; // velocidade 
	float _ax, _ay, _az; //acelercao

public:
	Particle(float life, float fade, float red, float green, float blue, float x, float y, float z, float vx, float vy, float vz, float ax, float ay, float az);
	void update(double timePassed);
	void reset(float life, float fade, float red, float green, float blue, float x, float y, float z, float vx, float vy, float vz, float ax, float ay, float az);
	float getLife();
	
	float getX();
	float getY();
	float getZ();
	float getVx();
	float getVy();
	float getVz();
	float getAx();
	float getAy();
	float getAz();
	float getRed();
	float getGreen();
	float getBlue();
	
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void setVx(float vx);
	void setVy(float vy);
	void setVz(float vz);



};

#endif