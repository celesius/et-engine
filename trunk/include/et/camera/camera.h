/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

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
		void windowProjection(const vec2& windowSize);

		const vec3 position() const
			{ return _inverseModelViewMatrix[3].xyz(); }

		void setPosition(const vec3& pos);

		const quaternion orientation() const
			{ return matrixToQuaternion(_modelViewMatrix.mat3()); }

		void setOrientation(const vec3& pos);

		vec3 direction() const;
		void setDirection(const vec3& d);

		const mat4& modelViewMatrix() const 
			{ return _modelViewMatrix; }

		const mat4& projectionMatrix() const 
			{ return _projectionMatrix; }

		const mat4& modelViewProjectionMatrix() const
			{ return _mvpMatrix; }

		const mat4& inverseModelViewMatrix() const 
			{ return _inverseModelViewMatrix; }

		const mat4& inverseProjectionMatrix() const
			{ return _inverseProjectionMatrix; }

		const mat4& inverseModelViewProjectionMatrix() const
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

		bool upVectorLocked() const
			{ return _lockUpVector; }

		const vec3& lockedUpVector() const 
			{ return _upLocked; }

		const Frustum& frustum() const
			{ return _frustum; }

		ray castRay(const vec2& pt) const;

		void setModelViewMatrix(const mat4& m)
			{ _modelViewMatrix = m; modelViewUpdated(); }
		void setProjectionMatrix(const mat4& m)
			{ _projectionMatrix = m; projectionUpdated(); }

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

	typedef StaticDataStorage<mat4, 6> CubemapProjectionMatrixArray;
	CubemapProjectionMatrixArray cubemapMatrixProjectionArray(const mat4& proj, const vec3& point);
}