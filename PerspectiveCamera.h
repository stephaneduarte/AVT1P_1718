#ifndef __PERSPECTIVE_CAMERA__
#define __PERSPECTIVE_CAMERA__

#include "Camera.h"

class PerspectiveCamera : public Camera {
	private:
		float _fov;
		float _ratio;
		float _near;
		float _far;
	public:
		PerspectiveCamera(float fov, float ratio, float near, float far);
		void setCamera();
		void updateCamera(float posX, float posY, float posZ, float eyeX, float eyeY, float eyeZ, float upX, float upY, float upZ);
};

#endif