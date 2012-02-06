#include <iostream>
#include <et/camera/camera.h>

using namespace et;

Camera::Camera() : _modelViewMatrix(1.0f), _projectionMatrix(1.0f), _mvpMatrix(1.0f), _inverseModelViewMatrix(1.0f), 
	_inverseProjectionMatrix(1.0f),	_inverseMVPMatrix(1.0f), _lockUpVector(false)
{

}

void Camera::lookAt(const vec3& pos, const vec3& point, const vec3& up)
{
	vec3 d = normalize(point - pos);
	vec3 s = normalize(d.cross(up));
	vec3 u = normalize(s.cross(d));
	vec3 e(-dot(s, pos), -dot(u, pos), dot(d, pos));

	_modelViewMatrix[0] = vec4(s.x, u.x, -d.x, 0.0);
	_modelViewMatrix[1] = vec4(s.y, u.y, -d.y, 0.0);
	_modelViewMatrix[2] = vec4(s.z, u.z, -d.z, 0.0);
	_modelViewMatrix[3] = vec4(e.x, e.y,  e.z, 1.0);

	modelViewUpdated();
}

void Camera::perspectiveProjection(float fov, float aspect, float zNear, float zFar)
{
	_projectionMatrix = IDENTITY_MATRIX;

	float fHalfFOV = 0.5f * fov;
	float cotan = cos(fHalfFOV) / sin(fHalfFOV);
	float dz = zFar - zNear;

	_projectionMatrix[0][0] = cotan / aspect;
	_projectionMatrix[1][1] = cotan;
	_projectionMatrix[2][2] = -(zFar + zNear) / dz;
	_projectionMatrix[3][3] =  0.0f;
	_projectionMatrix[2][3] = -1.0f;
	_projectionMatrix[3][2] = -2.0f * zNear * zFar / dz;

	projectionUpdated();
}

void Camera::orthogonalProjection(float left, float right, float top, float bottom, float zNear, float zFar)
{
	_projectionMatrix = IDENTITY_MATRIX;

	_projectionMatrix[0][0] = 2.0f / (right - left);
	_projectionMatrix[1][1] = 2.0f / (top - bottom);
	_projectionMatrix[2][2] = -2.0f / (zFar - zNear);

	_projectionMatrix[3][0] = -(right + left) / (right - left);
	_projectionMatrix[3][1] = -(top + bottom) / (top - bottom);
	_projectionMatrix[3][2] = -(zFar + zNear) / (zFar - zNear);

	projectionUpdated();
}

void Camera::setPosition(const vec3& p)
{
	_modelViewMatrix[3] = vec4(-_modelViewMatrix.rotationMultiply(p), _modelViewMatrix[3][3]);
	modelViewUpdated();
}

void Camera::setDirection(const vec3& d)
{
	vec3 u = up();
	vec3 s = d.cross(u);
	vec3 p = position();
	vec3 e(-dot(s, p), -dot(u, p), dot(d, p));
	_modelViewMatrix[0] = vec4(s.x, u.x, -d.x, 0.0);
	_modelViewMatrix[1] = vec4(s.y, u.y, -d.y, 0.0);
	_modelViewMatrix[2] = vec4(s.z, u.z, -d.z, 0.0);
	_modelViewMatrix[3] = vec4(e.x, e.y,  e.z, 1.0);
	modelViewUpdated();
}

void Camera::setOrientation(const vec3&)
{

}

vec3 Camera::direction() const
{
	return _modelViewMatrix.column(2).xyz();
}

vec3 Camera::up() const
{
	return _modelViewMatrix.column(1).xyz();
}

vec3 Camera::side() const
{
	return _modelViewMatrix.column(0).xyz();
}

vec3 Camera::invDirection() const
{
	return _inverseModelViewMatrix.column(2).xyz();
}

vec3 Camera::invUp() const
{
	return _inverseModelViewMatrix.column(1).xyz();
}

vec3 Camera::invSide() const
{
	return _inverseModelViewMatrix.column(0).xyz();
}

float Camera::heading() const
{
	return -asin(_modelViewMatrix[1][2]);
}

void Camera::move(const vec3& dp)
{
	_modelViewMatrix[3] += vec4(_modelViewMatrix.rotationMultiply(dp), 0.0);
	modelViewUpdated();
}

void Camera::rotate(const vec3& axis)
{
	_modelViewMatrix *= rotationYXZMatrix(axis);
	modelViewUpdated();
}

void Camera::rotate(const quaternion& q)
{
	_modelViewMatrix *= q.toMatrix();
	modelViewUpdated();
}

ray Camera::castRay(const vec2& pt) const
{
	vec3 pos = position();
	return ray(pos, normalize(_inverseMVPMatrix * vec3(pt, 1.0) - pos));
}

void Camera::modelViewUpdated()
{
	if (_lockUpVector)
	{
		vec3 p = position();
		vec3 d = -direction();
		vec3 s = normalize(d.cross(_upLocked));
		vec3 u = normalize(s.cross(d));
		vec3 e(-dot(s, p), -dot(u, p), dot(d, p));
		_modelViewMatrix[0] = vec4(s.x, u.x, -d.x, 0.0);
		_modelViewMatrix[1] = vec4(s.y, u.y, -d.y, 0.0);
		_modelViewMatrix[2] = vec4(s.z, u.z, -d.z, 0.0);
		_modelViewMatrix[3] = vec4(e.x, e.y,  e.z, 1.0);
	}

	_inverseModelViewMatrix = _modelViewMatrix.inverse();
	updateMVP();
}

void Camera::projectionUpdated()
{
	_inverseProjectionMatrix = _projectionMatrix.inverse();
	updateMVP();
}

void Camera::updateMVP()
{
	_mvpMatrix = _modelViewMatrix * _projectionMatrix;
	_inverseMVPMatrix = _mvpMatrix.inverse();
	_frustum = Frustum(_mvpMatrix);
}

void Camera::lockUpVector(const vec3& u)
{
	_upLocked = u;
	_lockUpVector = true;
	modelViewUpdated();
}

void Camera::unlockUpVector()
{
	_lockUpVector = true;
}