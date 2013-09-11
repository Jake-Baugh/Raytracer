#include "Camera.h"

Camera::Camera()
{
	m_position	= DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_right		= DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_look		= DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_up		= DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

	float identityMatrix[] = {  1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f};

	m_viewMatrix		= DirectX::XMFLOAT4X4(identityMatrix);
	m_projMatrix	= DirectX::XMFLOAT4X4(identityMatrix);
}

Camera::~Camera()
{
}

DirectX::XMFLOAT3 Camera::getPosition() const
{
	return m_position;
}
DirectX::XMFLOAT3 Camera::getRight() const
{
	return m_right;
}
DirectX::XMFLOAT3 Camera::getLook() const
{
	return m_look;
}
DirectX::XMFLOAT3 Camera::getUp() const
{
	return m_up;
}

DirectX::XMFLOAT4X4 Camera::getViewMatrix() const
{
	return m_viewMatrix;
}
DirectX::XMFLOAT4X4 Camera::getViewMatrixInv()
{
	DirectX::XMMATRIX xmView		= DirectX::XMLoadFloat4x4(&m_viewMatrix);
	DirectX::XMVECTOR xmDeterminant = DirectX::XMMatrixDeterminant(xmView);
	DirectX::XMMATRIX xmViewInv		= DirectX::XMMatrixInverse(&xmDeterminant, xmView);
	
	DirectX::XMFLOAT4X4 viewInv;
	DirectX::XMStoreFloat4x4(&viewInv, xmViewInv);

	return viewInv;
}
DirectX::XMFLOAT4X4 Camera::getProjMatrix() const
{
	return m_projMatrix;
}
DirectX::XMFLOAT4X4 Camera::getProjMatrixInv()
{
	DirectX::XMMATRIX xmProj		= DirectX::XMLoadFloat4x4(&m_projMatrix);
	DirectX::XMVECTOR xmDeterminant = DirectX::XMMatrixDeterminant(xmProj);
	DirectX::XMMATRIX xmProjInv		= DirectX::XMMatrixInverse(&xmDeterminant, xmProj);
	
	DirectX::XMFLOAT4X4 projInv;
	DirectX::XMStoreFloat4x4(&projInv, xmProjInv);

	return projInv;
}

void Camera::setLens(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	float zeroMatrix[16] = {0.0f};
	m_projMatrix = DirectX::XMFLOAT4X4(zeroMatrix);

	m_projMatrix._11 = 1/(aspectRatio * (tan(fieldOfView/2)));
	m_projMatrix._22 = 1/(tan(fieldOfView/2));
	m_projMatrix._33 = farPlane/(farPlane-nearPlane);
	m_projMatrix._34 = 1.0f;
	m_projMatrix._43 = (-nearPlane*farPlane)/(farPlane-nearPlane);
}

void Camera::strafe(float distance)
{
	m_position.x += m_right.x * distance;
	m_position.y += m_right.y * distance;
	m_position.z += m_right.z * distance;
}
void Camera::walk(float distance)
{
	m_position.x += m_look.x * distance;
	m_position.y += m_look.y * distance;
	m_position.z += m_look.z * distance;
}
void Camera::verticalWalk(float distance)
{
	m_position.y += distance;
}

void Camera::pitch(float angle)
{
	DirectX::XMMATRIX rotationMatrix;
	DirectX::XMVECTOR xmRight	= DirectX::XMLoadFloat3(&m_right);
	DirectX::XMVECTOR xmLook	= DirectX::XMLoadFloat3(&m_look);
	DirectX::XMVECTOR xmUp		= DirectX::XMLoadFloat3(&m_up);
	rotationMatrix = DirectX::XMMatrixRotationAxis(xmRight, angle);

	xmLook	= DirectX::XMVector3TransformNormal(xmLook, rotationMatrix);
	xmUp	= DirectX::XMVector3TransformNormal(xmUp, rotationMatrix);

	DirectX::XMStoreFloat3(&m_look, xmLook);
	DirectX::XMStoreFloat3(&m_up, xmUp);
}
void Camera::yaw(float angle)
{
	DirectX::XMVECTOR yAxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX rotationMatrix;

	DirectX::XMVECTOR xmRight	= DirectX::XMLoadFloat3(&m_right);
	DirectX::XMVECTOR xmLook	= DirectX::XMLoadFloat3(&m_look);
	DirectX::XMVECTOR xmUp		= DirectX::XMLoadFloat3(&m_up);

	rotationMatrix = DirectX::XMMatrixRotationAxis(yAxis, angle);
	xmRight	= DirectX::XMVector3TransformNormal(xmRight, rotationMatrix);
	xmLook	= DirectX::XMVector3TransformNormal(xmLook, rotationMatrix);
	xmUp	= DirectX::XMVector3TransformNormal(xmUp, rotationMatrix);

	DirectX::XMStoreFloat3(&m_right, xmRight);
	DirectX::XMStoreFloat3(&m_look, xmLook);
	DirectX::XMStoreFloat3(&m_up, xmUp);
}

void Camera::rebuildView()
{
	DirectX::XMVECTOR vLook	= DirectX::XMLoadFloat3(&m_look);
	DirectX::XMVECTOR vUp	= DirectX::XMLoadFloat3(&m_up);
	DirectX::XMVECTOR vRight	= DirectX::XMLoadFloat3(&m_right);
	
	vLook = DirectX::XMVector3Normalize(vLook);
	
	vUp = DirectX::XMVector3Cross(vLook, vRight);
	vUp = DirectX::XMVector3Normalize(vUp);
	
	vRight = DirectX::XMVector3Cross(vUp, vLook);
	vRight = DirectX::XMVector3Normalize(vRight);
	
	DirectX::XMVECTOR vPosition = DirectX::XMVectorSet( m_position.x*-1.0f, m_position.y*-1.0f, m_position.z*-1.0f, 1.0f);
	
	DirectX::XMVECTOR vResult;
	DirectX::XMFLOAT3 fResult;
	vResult = DirectX::XMVector3Dot(vPosition, vRight);
	DirectX::XMStoreFloat3(&fResult, vResult);
	float x = fResult.x;
	vResult = DirectX::XMVector3Dot(vPosition, vUp);
	DirectX::XMStoreFloat3(&fResult, vResult);
	float y = fResult.y;
	vResult = DirectX::XMVector3Dot(vPosition, vLook);
	DirectX::XMStoreFloat3(&fResult, vResult);
	float z = fResult.z;
	
	DirectX::XMStoreFloat3(&m_right, vRight);
	DirectX::XMStoreFloat3(&m_up, vUp);
	DirectX::XMStoreFloat3(&m_look, vLook);
	
	m_viewMatrix(0, 0) = m_right.x;
	m_viewMatrix(1, 0) = m_right.y;
	m_viewMatrix(2, 0) = m_right.z;
	m_viewMatrix(3, 0) = x;
		
	m_viewMatrix(0, 1) = m_up.x;
	m_viewMatrix(1, 1) = m_up.y;
	m_viewMatrix(2, 1) = m_up.z;
	m_viewMatrix(3, 1) = y;
		
	m_viewMatrix(0, 2) = m_look.x;
	m_viewMatrix(1, 2) = m_look.y;
	m_viewMatrix(2, 2) = m_look.z;
	m_viewMatrix(3, 2) = z;
		
	m_viewMatrix(0, 3) = 0.0f;
	m_viewMatrix(1, 3) = 0.0f;
	m_viewMatrix(2, 3) = 0.0f;
	m_viewMatrix(3, 3) = 1.0f;
}