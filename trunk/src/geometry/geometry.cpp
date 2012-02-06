#include <assert.h>
#include <et/geometry/geometry.h>

using namespace et;

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
	return quaternion(q0, q1, q2, q3).normalize();
}

void et::decomposeMatrix(mat4 mat, vec3& translation, quaternion& rotation, vec3& scale)
{
	translation = mat[2].xyz();
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
