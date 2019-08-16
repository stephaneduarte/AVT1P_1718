#include "PerspectiveCamera.h"
#include "Camera.h"
#include <stdio.h>
#include "AVTmathLib.h"

PerspectiveCamera::PerspectiveCamera(float fov, float ratio, float near, float far) {
	_fov = fov;
	_ratio = ratio;
	_near = near;
	_far = far;
}

//Camera 2
void PerspectiveCamera::setCamera() {
	loadIdentity(PROJECTION);
	perspective(_fov, _ratio, _near, _far);
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	lookAt(-10, 40, -10, 15, 5, 15, 0, 1, 0);
}

void PerspectiveCamera::updateCamera(float posX, float posY, float posZ, float eyeX, float eyeY, float eyeZ, float upX, float upY, float upZ) {
	loadIdentity(PROJECTION);
	perspective(_fov, _ratio, _near, _far);
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	lookAt(posX, posY, posZ, eyeX, eyeY, eyeZ, upX, upY, upZ);
}