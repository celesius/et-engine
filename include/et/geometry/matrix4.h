#pragma once

#include <et/geometry/vector4.h>
#include <et/geometry/matrix3.h>

namespace et
{
	template<typename T>
	struct matrix4
	{
		vector4<T> mat[4];

		matrix4()
		{
			mat[0] = vector4<T>(0.0f, 0.0f, 0.0f, 0.0f);
			mat[1] = vector4<T>(0.0f, 0.0f, 0.0f, 0.0f);
			mat[2] = vector4<T>(0.0f, 0.0f, 0.0f, 0.0f);
			mat[3] = vector4<T>(0.0f, 0.0f, 0.0f, 0.0f);
		}

		matrix4(const matrix4<T>& c)
		{
			mat[0] = c[0];
			mat[1] = c[1];
			mat[2] = c[2];
			mat[3] = c[3];
		}

		matrix4(matrix4<T>&& c)
		{
			mat[0] = c[0];
			mat[1] = c[1];
			mat[2] = c[2];
			mat[3] = c[3];
		}

		explicit matrix4(T s)
		{
			mat[0] = vector4<T>(   s, 0.0f, 0.0f, 0.0f);
			mat[1] = vector4<T>(0.0f,    s, 0.0f, 0.0f);
			mat[2] = vector4<T>(0.0f, 0.0f,    s, 0.0f);
			mat[3] = vector4<T>(0.0f, 0.0f, 0.0f,    s);
		}

		matrix4(const vector4<T>& c0, const vector4<T>& c1, const vector4<T>& c2, const vector4<T>& c3)
		{
			mat[0] = c0;
			mat[1] = c1;
			mat[2] = c2;
			mat[3] = c3;
		}

		matrix4(const matrix3<T>& transform, const vector3<T>& translation, bool explicitTranslation)
		{
			mat[0] = vec4(transform[0], 0);
			mat[1] = vec4(transform[1], 0);
			mat[2] = vec4(transform[2], 0);
			mat[3] = vec4(explicitTranslation ? translation : transform * translation, 1);
		}

		explicit matrix4(const matrix3<T>& m)
		{
			mat[0] = vector4<T>(m[0], 0.0f);
			mat[1] = vector4<T>(m[1], 0.0f);
			mat[2] = vector4<T>(m[2], 0.0f);
			mat[3] = vector4<T>(0.0f, 0.0f, 0.0f, 1.0);
		}

		matrix4& operator = (const matrix4<T>& c)
		{
			mat[0] = c[0];
			mat[1] = c[1];
			mat[2] = c[2];
			mat[3] = c[3];
			return *this;
		}
		
		matrix4 operator = (matrix4<T>&& c)
		{
			mat[0] = c[0];
			mat[1] = c[1];
			mat[2] = c[2];
			mat[3] = c[3];
			return *this;
		}
		
		inline const T* data() const
		{ return mat[0].data(); }  

		inline const void* raw() const
		{ return mat[0].raw(); }  

		inline const char* binary() const
		{ return mat[0].binary(); }  

		inline char* binary()
		{ return mat[0].binary(); }  

		inline T& operator ()(int i)
		{ return *(mat[0].data() + i); }

		inline const T& operator ()(int i) const 
		{ return *(mat[0].data() + i); }

		inline vector4<T>& operator [](int i)
		{ return mat[i];}

		inline const vector4<T>& operator [](int i) const
		{ return mat[i];}

		inline vector4<T> column(int c) const
		{ return vector4<T>( mat[0][c], mat[1][c], mat[2][c], mat[3][c]); }

		inline matrix4 operator * (T s) const
		{ return matrix4<T>(mat[0] * s, mat[1] * s, mat[2] * s, mat[3] * s); }

		inline matrix4 operator / (T s) const
		{ return matrix4<T>(mat[0] / s, mat[1] / s, mat[2] / s, mat[3] / s); }

		inline matrix4 operator + (const matrix4& m) const
		{ return matrix4(mat[0] + m.mat[0], mat[1] + m.mat[1], mat[2] + m.mat[2], mat[3] + m.mat[3]); }

		inline matrix4 operator - (const matrix4& m) const
		{ return matrix4(mat[0] - m.mat[0], mat[1] - m.mat[1], mat[2] - m.mat[2], mat[3] - m.mat[3]); }

		inline matrix4& operator /= (T m)
		{
			mat[0] /= m;
			mat[1] /= m;
			mat[2] /= m;
			mat[3] /= m; 
			return *this;
		}

		inline vector3<T> operator * (const vector3<T>& v) const
		{
			vector3<T> r;
			r.x = mat[0][0] * v.x + mat[1][0] * v.y + mat[2][0] * v.z + mat[3][0];
			r.y = mat[0][1] * v.x + mat[1][1] * v.y + mat[2][1] * v.z + mat[3][1];
			r.z = mat[0][2] * v.x + mat[1][2] * v.y + mat[2][2] * v.z + mat[3][2];
			T w = mat[0][3] * v.x + mat[1][3] * v.y + mat[2][3] * v.z + mat[3][3];
			return (w*w > 0) ? r / w : r;
		}

		inline vector4<T> operator * (const vector4<T>& v) const
		{
			return vector4<T>(
				mat[0][0] * v.x + mat[1][0] * v.y + mat[2][0] * v.z + mat[3][0] * v.w,
				mat[0][1] * v.x + mat[1][1] * v.y + mat[2][1] * v.z + mat[3][1] * v.w,
				mat[0][2] * v.x + mat[1][2] * v.y + mat[2][2] * v.z + mat[3][2] * v.w,
				mat[0][3] * v.x + mat[1][3] * v.y + mat[2][3] * v.z + mat[3][3] * v.w );
		}

		inline matrix4& operator += (const matrix4& m)
		{
			mat[0] += m.mat[0];
			mat[1] += m.mat[1];
			mat[2] += m.mat[2];
			mat[3] += m.mat[3];
			return *this;
		}

		inline matrix4& operator -= (const matrix4& m)
		{
			mat[0] -= m.mat[0];
			mat[1] -= m.mat[1];
			mat[2] -= m.mat[2];
			mat[3] -= m.mat[3];
			return *this;
		}

		inline matrix4& operator *= (const matrix4& m)
		{
			vector4<T> r0, r1, r2, r3;

			r0.x = mat[0].x*m.mat[0].x + mat[0].y*m.mat[1].x + mat[0].z*m.mat[2].x + mat[0].w*m.mat[3].x;
			r0.y = mat[0].x*m.mat[0].y + mat[0].y*m.mat[1].y + mat[0].z*m.mat[2].y + mat[0].w*m.mat[3].y;
			r0.z = mat[0].x*m.mat[0].z + mat[0].y*m.mat[1].z + mat[0].z*m.mat[2].z + mat[0].w*m.mat[3].z;
			r0.w = mat[0].x*m.mat[0].w + mat[0].y*m.mat[1].w + mat[0].z*m.mat[2].w + mat[0].w*m.mat[3].w;
			r1.x = mat[1].x*m.mat[0].x + mat[1].y*m.mat[1].x + mat[1].z*m.mat[2].x + mat[1].w*m.mat[3].x;
			r1.y = mat[1].x*m.mat[0].y + mat[1].y*m.mat[1].y + mat[1].z*m.mat[2].y + mat[1].w*m.mat[3].y;
			r1.z = mat[1].x*m.mat[0].z + mat[1].y*m.mat[1].z + mat[1].z*m.mat[2].z + mat[1].w*m.mat[3].z;
			r1.w = mat[1].x*m.mat[0].w + mat[1].y*m.mat[1].w + mat[1].z*m.mat[2].w + mat[1].w*m.mat[3].w;
			r2.x = mat[2].x*m.mat[0].x + mat[2].y*m.mat[1].x + mat[2].z*m.mat[2].x + mat[2].w*m.mat[3].x;
			r2.y = mat[2].x*m.mat[0].y + mat[2].y*m.mat[1].y + mat[2].z*m.mat[2].y + mat[2].w*m.mat[3].y;
			r2.z = mat[2].x*m.mat[0].z + mat[2].y*m.mat[1].z + mat[2].z*m.mat[2].z + mat[2].w*m.mat[3].z;
			r2.w = mat[2].x*m.mat[0].w + mat[2].y*m.mat[1].w + mat[2].z*m.mat[2].w + mat[2].w*m.mat[3].w;
			r3.x = mat[3].x*m.mat[0].x + mat[3].y*m.mat[1].x + mat[3].z*m.mat[2].x + mat[3].w*m.mat[3].x;
			r3.y = mat[3].x*m.mat[0].y + mat[3].y*m.mat[1].y + mat[3].z*m.mat[2].y + mat[3].w*m.mat[3].y;
			r3.z = mat[3].x*m.mat[0].z + mat[3].y*m.mat[1].z + mat[3].z*m.mat[2].z + mat[3].w*m.mat[3].z;
			r3.w = mat[3].x*m.mat[0].w + mat[3].y*m.mat[1].w + mat[3].z*m.mat[2].w + mat[3].w*m.mat[3].w;

			mat[0] = r0;
			mat[1] = r1;
			mat[2] = r2;
			mat[3] = r3;

			return *this;
		}

		inline matrix4 operator * (const matrix4& m) const
		{
			vector4<T> r0, r1, r2, r3;

			r0.x = mat[0].x*m.mat[0].x + mat[0].y*m.mat[1].x + mat[0].z*m.mat[2].x + mat[0].w*m.mat[3].x;
			r0.y = mat[0].x*m.mat[0].y + mat[0].y*m.mat[1].y + mat[0].z*m.mat[2].y + mat[0].w*m.mat[3].y;
			r0.z = mat[0].x*m.mat[0].z + mat[0].y*m.mat[1].z + mat[0].z*m.mat[2].z + mat[0].w*m.mat[3].z;
			r0.w = mat[0].x*m.mat[0].w + mat[0].y*m.mat[1].w + mat[0].z*m.mat[2].w + mat[0].w*m.mat[3].w;

			r1.x = mat[1].x*m.mat[0].x + mat[1].y*m.mat[1].x + mat[1].z*m.mat[2].x + mat[1].w*m.mat[3].x;
			r1.y = mat[1].x*m.mat[0].y + mat[1].y*m.mat[1].y + mat[1].z*m.mat[2].y + mat[1].w*m.mat[3].y;
			r1.z = mat[1].x*m.mat[0].z + mat[1].y*m.mat[1].z + mat[1].z*m.mat[2].z + mat[1].w*m.mat[3].z;
			r1.w = mat[1].x*m.mat[0].w + mat[1].y*m.mat[1].w + mat[1].z*m.mat[2].w + mat[1].w*m.mat[3].w;

			r2.x = mat[2].x*m.mat[0].x + mat[2].y*m.mat[1].x + mat[2].z*m.mat[2].x + mat[2].w*m.mat[3].x;
			r2.y = mat[2].x*m.mat[0].y + mat[2].y*m.mat[1].y + mat[2].z*m.mat[2].y + mat[2].w*m.mat[3].y;
			r2.z = mat[2].x*m.mat[0].z + mat[2].y*m.mat[1].z + mat[2].z*m.mat[2].z + mat[2].w*m.mat[3].z;
			r2.w = mat[2].x*m.mat[0].w + mat[2].y*m.mat[1].w + mat[2].z*m.mat[2].w + mat[2].w*m.mat[3].w;

			r3.x = mat[3].x*m.mat[0].x + mat[3].y*m.mat[1].x + mat[3].z*m.mat[2].x + mat[3].w*m.mat[3].x;
			r3.y = mat[3].x*m.mat[0].y + mat[3].y*m.mat[1].y + mat[3].z*m.mat[2].y + mat[3].w*m.mat[3].y;
			r3.z = mat[3].x*m.mat[0].z + mat[3].y*m.mat[1].z + mat[3].z*m.mat[2].z + mat[3].w*m.mat[3].z;
			r3.w = mat[3].x*m.mat[0].w + mat[3].y*m.mat[1].w + mat[3].z*m.mat[2].w + mat[3].w*m.mat[3].w;

			return matrix4<T>(r0, r1, r2, r3);  
		}

		inline T determinant() const
		{
			const T& a10 = mat[1].x;
			const T& a11 = mat[1].y;
			const T& a12 = mat[1].z;
			const T& a13 = mat[1].w;
			const T& a20 = mat[2].x; 
			const T& a21 = mat[2].y; 
			const T& a22 = mat[2].z; 
			const T& a23 = mat[2].w; 
			const T& a30 = mat[3].x;
			const T& a31 = mat[3].y;
			const T& a32 = mat[3].z;
			const T& a33 = mat[3].w;

			return mat[0].x * (a11 * (a22*a33 - a23*a32) +
				a12 * (a31*a23 - a21*a33) +
				a13 * (a21*a32 - a22*a31))+
				mat[0].y * (a10 * (a23*a32 - a22*a33) +
				a20 * (a12*a33 - a13*a32) +
				a30 * (a13*a22 - a12*a23))+
				mat[0].z * (a10 * (a21*a33 - a31*a23) +
				a11 * (a30*a23 - a20*a33) +
				a13 * (a20*a31 - a21*a30))+ 
				mat[0].w * (a10 * (a22*a31 - a21*a32) +
				a11 * (a20*a32 - a30*a22) +
				a12 * (a21*a30 - a20*a31));
		}

		inline matrix3<T> mat3() const
		{
			return matrix3<T>(mat[0].xyz(), mat[1].xyz(), mat[2].xyz());
		}

		inline matrix3<T> subMatrix(int r, int c) const
		{
			matrix3<T> m;

			int i = 0; 
			for (int y = 0; y < 4; ++y)
			{
				if (y != r)
				{
					int j = 0;
					if (0 != c) m[i][j++] = mat[y].x;
					if (1 != c) m[i][j++] = mat[y].y;
					if (2 != c) m[i][j++] = mat[y].z;
					if (3 != c) m[i][j++] = mat[y].w;
					++i;
				} 
			}
			return m;
		}

		inline matrix4<T> adjugateMatrix() const
		{
			matrix4<T> m;

			m[0].x = subMatrix(0, 0).determinant();
			m[0].y = -subMatrix(1, 0).determinant();
			m[0].z = subMatrix(2, 0).determinant();
			m[0].w = -subMatrix(3, 0).determinant();
			m[1][0] = -subMatrix(0, 1).determinant();
			m[1][1] = subMatrix(1, 1).determinant();
			m[1][2] = -subMatrix(2, 1).determinant();
			m[1][3] = subMatrix(3, 1).determinant();
			m[2][0] = subMatrix(0, 2).determinant();
			m[2][1] = -subMatrix(1, 2).determinant();
			m[2][2] = subMatrix(2, 2).determinant();
			m[2][3] = -subMatrix(3, 2).determinant();
			m[3][0] = -subMatrix(0, 3).determinant();
			m[3][1] = subMatrix(1, 3).determinant();
			m[3][2] = -subMatrix(2, 3).determinant();
			m[3][3] = subMatrix(3, 3).determinant();

			return m;
		}

		inline vector3<T> rotationMultiply(const vector3<T>& v) const
		{
			return vector3<T>(
				mat[0][0] * v.x + mat[1][0] * v.y + mat[2][0] * v.z, 
				mat[0][1] * v.x + mat[1][1] * v.y + mat[2][1] * v.z,
				mat[0][2] * v.x + mat[1][2] * v.y + mat[2][2] * v.z);
		}

		inline matrix4<T> inverse() const
		{
			T det = determinant();
			return (det * det > 0) ? adjugateMatrix() / det : matrix4<T>(0);
		}

		inline bool isModelViewMatrix()
		{
			vector3<T>c1 = vector3<T>(mat[0][0], mat[1][0], mat[2][0]);
			vector3<T>c2 = vector3<T>(mat[0][1], mat[1][1], mat[2][1]);
			vector3<T>c3 = vector3<T>(mat[0][2], mat[1][2], mat[2][2]);
			vector3<T>c4 = vector3<T>(mat[0][3], mat[1][3], mat[2][3]);
			T l1 = c1.length();
			T l2 = c2.length();
			T l3 = c3.length();
			T l4 = c4.length();
			T c1dotc2 = c1.dot(c2);
			T c1dotc3 = c1.dot(c3);
			T c2dotc3 = c2.dot(c3);
			const T one = T(1);
			return (l1 == one) && (l2 = one) && (l3 = one) && (l4 = 0) && (c1dotc2 == 0) && (c1dotc3 == 0) && (c2dotc3 == 0);
		}

		inline matrix4 scaleWithPreservedPosition(const vector3<T>& p)
		{
			vector4<T> r1 = vector4<T>(mat[0].xyz() * p, mat[0].w);
			vector4<T> r2 = vector4<T>(mat[1].xyz() * p, mat[1].w);
			vector4<T> r3 = vector4<T>(mat[2].xyz() * p, mat[2].w);
			return matrix4(r1, r2, r3, mat[3]);
		}

		inline matrix4 rotateWithPreservedPosition(const vector3<T>& a)
		{
			matrix4 r = (*this) * _rotationYXZMatrix(a.x, a.y, a.z);
			r[3] = mat[3];
			return r;
		}

	};

	template <typename T>
	inline matrix4<T> _transMatrix(T x, T y, T z)
	{
		matrix4<T> M;
		M[0][0] = M[1][1] = M[2][2] = M[3][3] = T(1);
		M[3][0] = x;
		M[3][1] = y;
		M[3][2] = z;
		return M;
	}

	template <typename T>
	inline matrix4<T> _transScaleMatrix(T tx, T ty, T tz, T sx, T sy, T sz)
	{
		matrix4<T> M;
		M[0][0] = sx;
		M[1][1] = sy;
		M[2][2] = sz;
		M[3][3] = T(1);

		M[3][0] = tx;
		M[3][1] = ty;
		M[3][2] = tz;
		return M;
	}

	template <typename T>
	inline matrix4<T> _scaleMatrix(T x, T y, T z)
	{
		matrix4<T> M;
		M[0][0] = x;
		M[1][1] = y;
		M[2][2] = z;
		M[3][3] = T(1);
		return M;
	}

	template <typename T>
	inline matrix4<T> _rotationYXZMatrix(T x, T y, T z)
	{
		matrix4<T> m(static_cast<T>(1));

		float sx = sin(x);
		float cx = cos(x);
		float sy = sin(y);
		float cy = cos(y);
		float sz = sin(z);
		float cz = cos(z);

		m[0][0] =  cz*cy - sz*sx*sy; m[0][1] = -cx*sz; m[0][2] = cz*sy + sz*sx*cy;
		m[1][0] =  sz*cy + cz*sx*sy; m[1][1] =  cx*cz; m[1][2] = sz*sy - cz*sx*cy;
		m[2][0] = -cx*sy;            m[2][1] =  sx;    m[2][2] = cx*cy;           

		return m;
	}

	template <typename T>
	inline matrix4<T> _translationRotationYXZMatrix(T tx, T ty, T tz, T rx, T ry, T rz)
	{
		matrix4<T> m;

		float sx = sin(rx);
		float cx = cos(rx);
		float sy = sin(ry);
		float cy = cos(ry);
		float sz = sin(rz);
		float cz = cos(rz);

		m[0][0] =  cz*cy - sz*sx*sy; m[0][1] = -cx*sz; m[0][2] = cz*sy + sz*sx*cy;
		m[1][0] =  sz*cy + cz*sx*sy; m[1][1] =  cx*cz; m[1][2] = sz*sy - cz*sx*cy;
		m[2][0] = -cx*sy;            m[2][1] =  sx;    m[2][2] = cx*cy;           
		m[3][0] =  tx;				 m[3][1] =  ty;    m[3][2] = tz;
		m[3][3] = 1;

		return m;
	}

	template <typename T>
	inline matrix4<T> _rotationScaleMatrix(T rx, T ry, T rz, T scx, T scy, T scz)
	{
		matrix4<T> m;

		float sx = sin(rx);
		float cx = cos(rx);
		float sy = sin(ry);
		float cy = cos(ry);
		float sz = sin(rz);
		float cz = cos(rz);

		m[0][0] = scx * (cz*cy - sz*sx*sy); 
		m[0][1] = scy * (-cx*sz); 
		m[0][2] = scz * (cz*sy + sz*sx*cy);

		m[1][0] = scx * (sz*cy + cz*sx*sy);
		m[1][1] = scy * (cx*cz); 
		m[1][2] = scz * (sz*sy - cz*sx*cy);

		m[2][0] = scx * (-cx*sy);
		m[2][1] = scy * (sx);
		m[2][2] = scz * (cx*cy);
		m[3][3] = 1;

		return m;
	}

	template <typename T>
	inline matrix4<T> _transformYXZMatrix(T tx, T ty, T tz, T rx, T ry, T rz)
	{
		matrix4<T> m;

		float sx = sin(rx);
		float cx = cos(rx);
		float sy = sin(ry);
		float cy = cos(ry);
		float sz = sin(rz);
		float cz = cos(rz);

		m[0][0] =  cz*cy - sz*sx*sy; m[0][1] = -cx*sz; m[0][2] = cz*sy + sz*sx*cy;
		m[1][0] =  sz*cy + cz*sx*sy; m[1][1] =  cx*cz; m[1][2] = sz*sy - cz*sx*cy;
		m[2][0] = -cx*sy;            m[2][1] =  sx;    m[2][2] = cx*cy;           
		m[3][0] = tx;                m[3][1] =  ty;    m[3][2] = tz; 

		m[3][3] = 1;

		return m;
	}

	template <typename T>
	inline matrix4<T> _orientationForNormal(const vector3<T>& n) 
	{
		vector3<T> up = normalize(n);
		T theta = asin(up.y) - HALF_PI;
		T phi = atan2(up.z, up.x) + HALF_PI;
		T csTheta = cos(theta);
		vector3<T> side2(csTheta * cos(phi), sin(theta), csTheta * sin(phi));
		vector3<T> side1 = up.cross(side2);

		matrix4<T> result;
		result[0].xyz() = vector3<T>(side1.x, up.x, side2.x);
		result[1].xyz() = vector3<T>(side1.y, up.y, side2.y);
		result[2].xyz() = vector3<T>(side1.z, up.z, side2.z);
		return result;
	}

}
