#ifndef __ORTHO_CAMERA__
#define __ORTHO_CAMERA__

#include "Camera.h"

class OrthoCamera : public Camera {
	private:
		double _right;
		double _left;
		double _top;
		double _bottom;
		double _near;
		double _far;

	public:
		OrthoCamera(double right, double left, double bottom, double top, double near, double far);
		void setCamera();
};

#endif