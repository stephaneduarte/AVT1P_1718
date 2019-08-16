#ifndef __CAMERA__
#define __CAMERA__

class Camera {
	protected:
		float _posX;
		float _posY;
		float _posZ;
	public:
		Camera();
		void setPosition(float posX, float posY, float posZ);
		void setCamera();
};

#endif