#include "Light.h"

Light::Light() {
	_state = false;
}

Light::Light(int id) {
	_id = id;
}

int Light::getId() {
	return _id;
}

bool Light::getState() {
	return _state;
}

bool Light::getIsLocal() { 
	return _isLocal; 
}

bool Light::getIsSpot() { 
	return _isSpot; 
}

void Light::setState(bool state) {
	_state = state;
}

void Light::setIsLocal(bool isLocal) { 
	_isLocal = isLocal; 
}

void Light::setIsSpot(bool isSpot) { 
	_isSpot = isSpot; 
}

void Light::setCutOff(float cut_off) { 
	_cut_off = cut_off; 
}

void Light::setExponent(float exponent) { 
	_exponent = exponent; 
}

void Light::setPosition(const vec4& position) {
	_position.set(position.getX(), position.getY(), position.getZ(), position.getW());
}

void Light::setPosition(float x, float y, float z, float w) {
	_position.set(x, y, z, w);
}

void Light::setDirection(const vec4& direction) {
	_direction.set(direction.getX(), direction.getY(), direction.getZ(), direction.getW());
}

void Light::setDirection(float x, float y, float z, float w) {
	_direction.set(x, y, z, w);
}

void Light::setAmbient(const vec4& ambient) {
	_ambient.set(ambient.getX(), ambient.getY(), ambient.getZ(), ambient.getW());
}

void Light::setAmbient(float x, float y, float z, float w) {
	_ambient.set(x, y, z, w);
}

void Light::setDiffuse(const vec4& diffuse) {
	_diffuse.set(diffuse.getX(), diffuse.getY(), diffuse.getZ(), diffuse.getW());
}

void Light::setDiffuse(float x, float y, float z, float w) {
	_diffuse.set(x, y, z, w);
}

void Light::setSpecular(const vec4& specular) {
	_specular.set(specular.getX(), specular.getY(), specular.getZ(), specular.getW());
}

void Light::setSpecular(float x, float y, float z, float w) {
	_specular.set(x, y, z, w);
}

const vec4& Light::getPosition() { 
	return _position;
}

const vec4& Light::getDirection() { 
	return _direction;
}

float Light::getCutOff() { 
	return _cut_off; 
}

float Light::getExponent() { 
	return _exponent;
}

const vec4& Light::getAmbient() { 
	return _ambient; 
}

const vec4& Light::getDiffuse() { 
	return _diffuse; 
}

const vec4& Light::getSpecular() {
	return _specular; 
}

const float Light::getConstantAttenuation() const {
	return _constantAttenuation;
}

const float Light::getLinearAttenuation() const {
	return _linearAttenuation;
}

const float Light::getQuadraticAttenuation() const {
	return _quadraticAttenuation;
}

void Light::setConstantAttenuation(float constantAttenuation) {
	_constantAttenuation = constantAttenuation;
}

void Light::setLinearAttenuation(float linearAttenuation) {
	_linearAttenuation = linearAttenuation;
}

void Light::setQuadraticAttenuation(float quadraticAttenuation) {
	_quadraticAttenuation = quadraticAttenuation;
}