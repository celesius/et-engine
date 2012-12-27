/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/geometry/geometry.h>

using namespace et;

static const float fRandMax = static_cast<float>(RAND_MAX);

const vec3 et::unitX(1.0f, 0.0f, 0.0f);
const vec3 et::unitY(0.0f, 1.0f, 0.0f);
const vec3 et::unitZ(0.0f, 0.0f, 1.0f);

float et::sign(float s)
{
	return (s == 0.0f) ? 0.0f : s / fabs(s); 
}

float et::sign_nz(float s)
{
	return (s < 0.0f) ? -1.0f : 1.0f; 
}

vec3 et::randVector(float sx, float sy, float sz)
{
	float r0 = 2.0f * static_cast<float>(rand()) / fRandMax - 1.0f;
	float r1 = 2.0f * static_cast<float>(rand()) / fRandMax - 1.0f;
	float r2 = 2.0f * static_cast<float>(rand()) / fRandMax - 1.0f;
	return vec3(sx * r0, sy * r1, sz * r2);
}

float et::randf(float low, float up)
{
	return low + (up - low) * static_cast<float>(rand()) / fRandMax;
}

quaternion et::matrixToQuaternion(const mat3& r)
{
	float q0 = ( r[0][0] + r[1][1] + r[2][2] + 1.0f) / 4.0f;
	float q1 = ( r[0][0] - r[1][1] - r[2][2] + 1.0f) / 4.0f;
	float q2 = (-r[0][0] + r[1][1] - r[2][2] + 1.0f) / 4.0f;
	float q3 = (-r[0][0] - r[1][1] + r[2][2] + 1.0f) / 4.0f;
	q0 = (q0 < 0.0f) ? 0.0f : sqrt(q0);
	q1 = (q1 < 0.0f) ? 0.0f : sqrt(q1);
	q2 = (q2 < 0.0f) ? 0.0f : sqrt(q2);
	q3 = (q3 < 0.0f) ? 0.0f : sqrt(q3);
	if ((q0 >= q1) && (q0 >= q2) && (q0 >= q3))
	{
		q1 *= sign_nz(r[2][1] - r[1][2]);
		q2 *= sign_nz(r[0][2] - r[2][0]);
		q3 *= sign_nz(r[1][0] - r[0][1]);
	} 
	else if ((q1 >= q0) && (q1 >= q2) && (q1 >= q3))
	{
		q0 *= sign_nz(r[2][1] - r[1][2]);
		q2 *= sign_nz(r[1][0] + r[0][1]);
		q3 *= sign_nz(r[0][2] + r[2][0]);
	} 
	else if ((q2 >= q0) && (q2 >= q1) && (q2 >= q3))
	{
		q0 *= sign_nz(r[0][2] - r[2][0]);
		q1 *= sign_nz(r[1][0] + r[0][1]);
		q3 *= sign_nz(r[2][1] + r[1][2]);
	} 
	else if ((q3 >= q0) && (q3 >= q1) && (q3 >= q2))
	{
		q0 *= sign_nz(r[1][0] - r[0][1]);
		q1 *= sign_nz(r[2][0] + r[0][2]);
		q2 *= sign_nz(r[2][1] + r[1][2]);
	} 
	else 
	{
		assert(0 && "Unable to convert matrix to quaternion");
	}

	return normalize(quaternion(q0, q1, q2, q3));
}

void et::decomposeMatrix(mat4 mat, vec3& translation, quaternion& rotation, vec3& scale)
{
	translation = mat[3].xyz();
	scale = removeMatrixScale(mat);
	rotation = matrixToQuaternion(mat.mat3());
}

vec3 et::removeMatrixScaleRowMajor(mat3& mat)
{
	vec3& r0 = mat[0];
	vec3& r1 = mat[1];
	vec3& r2 = mat[2];
	vec3 scale(r0.length(), r1.length(), r2.length());
	r0 /= scale.x;
	r1 /= scale.y;
	r2 /= scale.z;
	return scale;
}

vec3 et::removeMatrixScale(mat3& mat)
{
	vec3& r0 = mat[0];
	vec3& r1 = mat[1];
	vec3& r2 = mat[2];
	vec3 scale(vec3(r0.x, r1.x, r2.x).length(), vec3(r0.y, r1.y, r2.y).length(), vec3(r0.z, r1.z, r2.z).length());
	r0 /= scale.x;
	r1 /= scale.y;
	r2 /= scale.z;
	return scale;
}

vec3 et::removeMatrixScale(mat4& mat)
{
	vec3& r0 = mat[0].xyz();
	vec3& r1 = mat[1].xyz();
	vec3& r2 = mat[2].xyz();
	vec3 scale(vec3(r0.x, r1.x, r2.x).length(), vec3(r0.y, r1.y, r2.y).length(), vec3(r0.z, r1.z, r2.z).length());
	r0 /= scale.x;
	r1 /= scale.y;
	r2 /= scale.z;
	return scale;
}

vec3ub et::vec3fto3ubscaled(const vec3 &fv)
{
	return vec3ub(
		static_cast<unsigned char>(clamp<float>(0.5f + 0.5f * fv.x, 0.0, 1.0) * 255.0f), 
		static_cast<unsigned char>(clamp<float>(0.5f + 0.5f * fv.y, 0.0, 1.0) * 255.0f),
		static_cast<unsigned char>(clamp<float>(0.5f + 0.5f * fv.z, 0.0, 1.0) * 255.0f));

}

vec3ub et::vec3fto3ublinear(const vec3& fv)
{
	return vec3ub(
		static_cast<unsigned char>(clamp(fv.x * 255.0f, 0.0f, 255.0f)), 
		static_cast<unsigned char>(clamp(fv.y * 255.0f, 0.0f, 255.0f)),
		static_cast<unsigned char>(clamp(fv.z * 255.0f, 0.0f, 255.0f)));

}

vec4ub et::vec4f_to_4ub(const vec4 &fv)
{
	return vec4ub( (unsigned char)(clamp<float>(0.5f + 0.5f * fv.x, 0.0, 1.0) * 255), 
		(unsigned char)(clamp<float>(0.5f + 0.5f * fv.y, 0.0, 1.0) * 255), 
		(unsigned char)(clamp<float>(0.5f + 0.5f * fv.z, 0.0, 1.0) * 255),
		(unsigned char)(clamp<float>(0.5f + 0.5f * fv.z, 0.0, 1.0) * 255) );
}


mat4 et::rotation2DMatrix(float angle)
{	
	float ca = cos(angle);
	float sa = sin(angle);
	mat4 m;
	m[0][0] = ca; m[0][1] = -sa;
	m[1][0] = sa; m[1][1] =  ca;
	m[2][2] = m[3][3] = 1.0f;
	return m;
}

mat4 et::transform2DMatrix(float a, const vec2& scale, const vec2& translate)
{
	float ca = cos(a);
	float sa = sin(a);
	mat4 m;
	m[0][0] = ca * scale.x; m[0][1] = -sa * scale.y;
	m[1][0] = sa * scale.x; m[1][1] =  ca * scale.y;
	m[2][2] = 1.0f;
	m[3][0] = translate.x;
	m[3][1] = translate.y;
	m[3][3] = 1.0f;
	return m;
}

mat3 et::rotation2DMatrix3(float angle)
{	
	float ca = cos(angle);
	float sa = sin(angle);
	mat3 m;
	m[0][0] = ca; m[0][1] = -sa;
	m[1][0] = sa; m[1][1] =  ca;
	m[2][2] = m[3][3] = 1.0f;
	return m;
}

template <>
vector2<float> et::bezierCurve(const std::vector< vector2<float> >& points, float time)
{
	assert(points.size() > 0);
	
	if (points.size() == 1)
	{
		return points.front();
	}
	else if (points.size() == 2)
	{
		return mix(points.front(), points.back(), time);
	}
	else
	{
		std::vector< vector2<float> > firstPoints(points.size() - 1);
		std::vector< vector2<float> > lastPoints(points.size() - 1);
		
		for (size_t i = 0; i < points.size() - 1; ++i)
		{
			firstPoints[i] = points[i];
			lastPoints[i] = points[i+1];
		}
		
		return mix( bezierCurve(firstPoints, time), bezierCurve(lastPoints, time), time );
	}
	
	return vector2<float>(0.0f);
}
