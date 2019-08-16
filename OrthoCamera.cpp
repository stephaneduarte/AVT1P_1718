#include "OrthoCamera.h"
#include "Camera.h"
#include <stdio.h>
#include "AVTmathLib.h"

OrthoCamera::OrthoCamera(double right, double left, double bottom, double top, double near, double far) {
	_right = right;
	_left = left;
	_bottom = bottom;
	_top = top;
	_near = near;
	_far = far;
}

void OrthoCamera::setCamera() {
	loadIdentity(PROJECTION);
	ortho(_right, _left, _bottom, _top, _near, _far);
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	lookAt(0, 50, 0, 0, 0, 0, 1, 0, 0);
}