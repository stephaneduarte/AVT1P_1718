#ifndef __LIGHT__
#define __LIGHT__

#include "vec4.h"

class Light
{
protected:
	int _id;
	bool _state;
	bool _isLocal;
	bool _isSpot;
	vec4 _ambient;
	vec4 _diffuse;
	vec4 _specular;
	vec4 _position;
	vec4 _direction;
	float _cut_off;
	float _exponent;
	float _constantAttenuation;
	float _linearAttenuation;
	float _quadraticAttenuation;

public:
	Light();
	Light(int id);

	int getId();

	bool getState();
	bool getIsLocal();
	bool getIsSpot();

	void setState(bool state);
	void setIsLocal(bool isLocal);
	void setIsSpot(bool isSpot);

	void setCutOff(float cut_off);
	void setExponent(float exponent);
	void setPosition(const vec4& position);
	void setDirection(const vec4& direction);
	void setAmbient(const vec4& ambient);
	void setDiffuse(const vec4& diffuse);
	void setSpecular(const vec4& specular);
	void setPosition(float x, float y, float z, float w);
	void setDirection(float x, float y, float z, float w);
	void setAmbient(float x, float y, float z, float w);
	void setDiffuse(float x, float y, float z, float w);
	void setSpecular(float x, float y, float z, float w);

	const vec4& getPosition();
	const vec4& getDirection();
	float getCutOff();
	float getExponent();
	const vec4& getAmbient();
	const vec4& getDiffuse();
	const vec4& getSpecular();
	const float getConstantAttenuation() const;
	const float getLinearAttenuation() const;
	const float getQuadraticAttenuation() const;
	void setConstantAttenuation(float constantAttenuation);
	void setLinearAttenuation(float linearAttenuation);
	void setQuadraticAttenuation(float quadraticAttenuation);
};

#endif
