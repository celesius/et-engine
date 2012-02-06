#pragma once

#include <et/camera/frustum.h>

namespace et
{
	class Camera
	{
	public:
		Camera();

		void lookAt(const vec3& pos, const vec3& point = vec3(0.0f), const vec3& up = vec3(0.0f, 1.0f, 0.0f));
		void perspectiveProjection(float fov, float aspect, float zNear, float zFar);
		void orthogonalProjection(float left, float right, float top, float bottom, float zNear, float zFar);

		inline const vec3 position() const
			{ return _inverseModelViewMatrix[3].xyz(); }

		void setPosition(const vec3& pos);

		inline const quaternion orientation() const
			{ return matrixToQuaternion(_modelViewMatrix.mat3()); }

		void setOrientation(const vec3& pos);

		vec3 direction() const;
		void setDirection(const vec3& d);

		inline const mat4& modelViewMatrix() const 
			{ return _modelViewMatrix; }

		inline const mat4& projectionMatrix() const 
			{ return _projectionMatrix; }

		inline const mat4& modelViewProjectionMatrix() const
			{ return _mvpMatrix; }

		inline const mat4& inverseModelViewMatrix() const 
			{ return _inverseModelViewMatrix; }

		inline const mat4& inverseProjectionMatrix() const
			{ return _inverseProjectionMatrix; }

		inline const mat4& inverseModelViewProjectionMatrix() const
			{ return _inverseMVPMatrix; }

		vec3 up() const;
		vec3 side() const;

		vec3 invDirection() const;
		vec3 invUp() const;
		vec3 invSide() const;

		float heading() const;

		void move(const vec3& dp);

		void rotate(const quaternion& q);
		void rotate(const vec3& axis);

		void lockUpVector(const vec3& u);
		void unlockUpVector();

		inline const Frustum& frustum() const
			{ return _frustum; }

		ray castRay(const vec2& pt) const;

	private:
		void modelViewUpdated();
		void projectionUpdated();
		void updateMVP();

	private:
		mat4 _modelViewMatrix;
		mat4 _projectionMatrix;
		mat4 _mvpMatrix;

		mat4 _inverseModelViewMatrix;
		mat4 _inverseProjectionMatrix;
		mat4 _inverseMVPMatrix;

		Frustum _frustum;
		vec3 _upLocked;
		bool _lockUpVector;
	};

}