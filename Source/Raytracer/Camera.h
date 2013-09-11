#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	~Camera();

	DirectX::XMFLOAT3 getPosition() const;
	DirectX::XMFLOAT3 getRight() const;
	DirectX::XMFLOAT3 getLook() const;
	DirectX::XMFLOAT3 getUp() const;

	DirectX::XMFLOAT4X4 getViewMatrix() const;
	DirectX::XMFLOAT4X4 getViewMatrixInv();
	DirectX::XMFLOAT4X4 getProjMatrix() const;
	DirectX::XMFLOAT4X4 getProjMatrixInv();

	void setLens(float p_fieldOfView, float p_aspectRatio, float p_nearPlane, float p_farPlane);

	void strafe(float p_distance);
	void walk(float p_distance);
	void verticalWalk(float p_distance);

	void pitch(float p_angle);
	void yaw(float p_angle);

	void rebuildView();
private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_right;
	DirectX::XMFLOAT3 m_look;
	DirectX::XMFLOAT3 m_up;

	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projMatrix;
};

#endif //RAYTRACER_CAMERA_H