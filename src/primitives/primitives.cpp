/*
 * This file is part of `et engine`
 * Copyright 2009-2012 by Sergey Reznik
 * Please, do not modify contents without approval.
 *
 */

#include <et/opengl/opengl.h>
#include <et/core/containers.h>
#include <et/primitives/primitives.h>

using namespace et;

inline size_t getIndex(size_t u, size_t v, size_t u_sz, size_t v_sz)
		{ return clamp<size_t>(u, 0, u_sz - 1) + clamp<size_t>(v, 0, v_sz - 1) * u_sz; }

size_t Primitives::indexCountForRegularMesh(const vec2i& meshSize, size_t geometryType)
{
	if (geometryType == GL_TRIANGLES)
		return ( (meshSize.x > 1) ? meshSize.x - 1 : 1 ) * ( (meshSize.y > 1) ? meshSize.y - 1 : 1) * 6;

	if (geometryType == GL_TRIANGLE_STRIP)
		return ( (meshSize.y > 1) ? meshSize.y - 1 : 1) * (2 * meshSize.x + 1) - 1;

	if (geometryType == GL_POINTS)
		return meshSize.square();

	return 0;
}

void Primitives::createPhotonMap(DataStorage<vec2>& buffer, const vec2i& density)
{
	int numPhotons = density.square();

	buffer.fitToSize(numPhotons);

	vec2 texel = vec2(1.0f / density.x, 1.0f / density.y);
	vec2 dxdy = vec2(0.5f / density.x, 0.5f / density.y);

	for (int i = 0; i < density.y; ++i)
		for (int j = 0; j < density.x; ++j)
			buffer.push_back(vec2(j * texel.x, i * texel.y) + dxdy);
}

void Primitives::createSphere(VertexArray::Pointer data, float radius, const vec2i& density, const vec3& center, const vec2& hemiSphere)
{ 
	size_t lastIndex = data->size();
	data->increase(density.square());

	VertexDataChunk pos_c = data->chunk(Usage_Position);
	VertexDataChunk norm_c = data->chunk(Usage_Normal);
	VertexDataChunk tex_c = data->chunk(Usage_TexCoord0);
	RawDataAcessor<vec3> pos = pos_c.valid() ? pos_c.accessData<vec3>(lastIndex) : RawDataAcessor<vec3>();
	RawDataAcessor<vec3> norm = norm_c.valid() ? norm_c.accessData<vec3>(lastIndex) : RawDataAcessor<vec3>();
	RawDataAcessor<vec2> tex = tex_c.valid() ? tex_c.accessData<vec2>(lastIndex) : RawDataAcessor<vec2>();

	bool hasPos = pos.valid();
	bool hasNorm = norm.valid();
	bool hasTex = tex.valid();

	float dPhi = hemiSphere.x * DOUBLE_PI / (density.x - 1.0f);
	float dTheta = hemiSphere.y * PI / (density.y - 1.0f);

	int counter = 0;
	float theta = -HALF_PI;
	for (int i = 0; i < density.y; ++i)
	{
		float phi = 0;
		for (int j = 0; j < density.x; ++j)
		{
			vec3 p = fromSpherical(theta, phi);
			if (hasPos)
				pos[counter] = center + p * radius;
			if (hasNorm)
				norm[counter] = p;
			if (hasTex)
				tex[counter] = vec2(static_cast<float>(j) / (density.x - 1), 1.0f - static_cast<float>(i) / (density.y - 1));
			phi += dPhi;
			++counter;
		}

		theta += dTheta;
	} 
}

void Primitives::createCylinder(VertexArray::Pointer data, float radius, float height, const vec2i& density, const vec3& center)
{
	size_t lastIndex = data->size();
	data->increase(density.square());

	VertexDataChunk pos_c = data->chunk(Usage_Position);
	VertexDataChunk norm_c = data->chunk(Usage_Normal);
	VertexDataChunk tex_c = data->chunk(Usage_TexCoord0);
	RawDataAcessor<vec3> pos = pos_c.valid() ? pos_c.accessData<vec3>(lastIndex) : RawDataAcessor<vec3>();
	RawDataAcessor<vec3> norm = norm_c.valid() ? norm_c.accessData<vec3>(lastIndex) : RawDataAcessor<vec3>();
	RawDataAcessor<vec2> tex = tex_c.valid() ? tex_c.accessData<vec2>(lastIndex) : RawDataAcessor<vec2>();

	bool hasPos = pos.valid();
	bool hasNorm = norm.valid();
	bool hasTex = tex.valid();

	float dPhi = DOUBLE_PI / (density.x - 1.0f);

	int counter = 0;
	float y = -0.5f * height;
	float dy = height / static_cast<float>(density.y - 1);

	for (int v = 0; v < density.y; ++v)
	{
		float phi = 0.0f;
		for (int j = 0; j < density.x; ++j)
		{
			vec3 p = vec3(cos(phi) * radius, y, sin(phi) * radius);

			if (hasPos)
				pos[counter] = center + p;

			if (hasNorm)
				norm[counter] = normalize(p * vec3(1.0f, 0.0f, 1.0f));

			if (hasTex)
				tex[counter] = vec2(phi / DOUBLE_PI, 1.0f - static_cast<float>(v) / (density.y - 1));

			phi += dPhi;
			++counter;
		}

		y += dy;
	}


}

void Primitives::createPlane(VertexArray::Pointer data, const vec3& normal, const vec2& size, const vec2i& density, 
		const vec3& center, const vec2& texCoordScale, const vec2& texCoordOffset)
{
	size_t lastIndex = data->size();
	data->increase(density.square());

	VertexDataChunk pos_c = data->chunk(Usage_Position);
	VertexDataChunk norm_c = data->chunk(Usage_Normal);
	VertexDataChunk tex_c = data->chunk(Usage_TexCoord0);
	RawDataAcessor<vec3> pos = pos_c.valid() ? pos_c.accessData<vec3>(lastIndex) : RawDataAcessor<vec3>();
	RawDataAcessor<vec3> norm = norm_c.valid() ? norm_c.accessData<vec3>(lastIndex) : RawDataAcessor<vec3>();
	RawDataAcessor<vec2> tex = tex_c.valid() ? tex_c.accessData<vec2>(lastIndex) : RawDataAcessor<vec2>();

	bool hasPos = pos.valid();
	bool hasNorm = norm.valid();
	bool hasTex = tex.valid();

	vec3 angles = toSpherical(normal);
	angles += vec3(-HALF_PI, HALF_PI, 0.0);

	vec3 normalized = normalize(normal);
	vec3 o2 = fromSpherical(angles.y, angles.x);
	vec3 o1 = cross(normalized, o2);
	o2 = cross(o1, normalized);

	vec3 v00 = -size.x * o1 - size.y * o2;
	vec3 v01 = -size.x * o1 + size.y * o2;
	vec3 v10 =  size.x * o1 - size.y * o2;
	vec3 v11 =  size.x * o1 + size.y * o2;

	vec3 n = plane(triangle(v00, v01, v10)).normal();
	vec2i dv(density.x < 2 ? 2 : density.x, density.y < 2 ? 2 : density.y);

	float dx = 1.0f / static_cast<float>(dv.x - 1);
	float dy = 1.0f / static_cast<float>(dv.y - 1);

	size_t counter = 0;
	for (int v = 0; v < dv.y; v++)
	{
		float py = v * dy;
		vec3 m0 = mix(v00, v10, py);
		vec3 m1 = mix(v01, v11, py);

		for (int u = 0; u < dv.x; u++)
		{
			float px = u * dx;
			if (hasPos)
				pos[counter] = center + mix(m0, m1, px);
			if (hasTex)
				tex[counter] = texCoordOffset + vec2(py, 1.0f - px) * texCoordScale;
			++counter;
		}

	}

	if (!hasNorm) return;

	for (int v = 0; v < dv.y; v++)
	{
		for (int u = 0; u < dv.x; u++)
		{
			size_t n00 = getIndex(u, v, dv.x, dv.y);
			size_t n01 = 0;
			size_t n10 = 0;

			vec3 p00 = pos[n00];
			vec3 p01;
			vec3 p10;

			if (u == dv.x - 1)
			{
				n01 = getIndex(u - 1,   v  , dv.x, dv.y);
				p01 = p00 + (p00 - pos[n01]);
			}
			else
			{
				n01 = getIndex(u + 1,   v  , dv.x, dv.y);
				p01 = pos[n01];
			} 
			if (v == dv.y -1 )
			{
				n10 = getIndex(  u,   v - 1, dv.x, dv.y);
				p10 = p00 + (p00 - pos[n10]); 
			}
			else
			{
				n10 = getIndex(  u,   v + 1, dv.x, dv.y);
				p10 = pos[n10];
			}
			norm[n00] = plane(triangle(p00, p10, p01)).normal();
		}
	}

	for (int v = 0; v < dv.y; v++)
	{
		for (int u = 0; u < dv.x; u++)
		{
			vec3 n = 
				1.0f * norm[getIndex(u-1, v-1, dv.x, dv.y)] + 
				2.0f * norm[getIndex(  u, v-1, dv.x, dv.y)] + 
				1.0f * norm[getIndex(u+1, v-1, dv.x, dv.y)] + 
				2.0f * norm[getIndex(u-1,   v, dv.x, dv.y)] + 
				4.0f * norm[getIndex(  u,   v, dv.x, dv.y)] + 
				2.0f * norm[getIndex(u+1,   v, dv.x, dv.y)] + 
				1.0f * norm[getIndex(u-1, v+1, dv.x, dv.y)] +
				2.0f * norm[getIndex(  u, v+1, dv.x, dv.y)] +
				1.0f * norm[getIndex(u+1, v+1, dv.x, dv.y)];

			norm[getIndex(u, v, dv.x, dv.y)] = normalize(n);
		}
	}
}

IndexType Primitives::buildTriangleStripIndexes(IndexArray::Pointer buffer, const vec2i& dim, IndexType index0, size_t offset)
{
	size_t k = offset;
	IndexType rowSize = static_cast<IndexType>(dim.x);
	IndexType colSize = static_cast<IndexType>(dim.y);

	for (IndexType v = 0; v < static_cast<IndexType>(dim.y - 1); ++v)
	{
		IndexType thisRow = index0 + v * rowSize;
		IndexType nextRow = thisRow + rowSize;

		if (v % 2 == 0)
		{
			for (IndexType u = 0; u < rowSize; ++u)
			{
				buffer->setIndex(u + thisRow, k++);
				buffer->setIndex(u + nextRow, k++);
			}
			if (v != colSize - 2)
				buffer->setIndex((rowSize - 1) + nextRow, k++);
		}
		else
		{
			for (IndexType u = rowSize - 1; ; --u)
			{
				buffer->setIndex(u + thisRow, k++);
				buffer->setIndex(u + nextRow, k++);
				if (u == 0)	break;
			}

			if (v != colSize - 2)
				buffer->setIndex(nextRow, k++);
		}
	}

	return k; 
}

IndexType Primitives::buildTrianglesIndexes(IndexArray::Pointer buffer, const vec2i& dim, IndexType vertexOffset, size_t indexOffset)
{
	size_t k = indexOffset;
	IndexType rowSize = static_cast<IndexType>(dim.x);
	IndexType colSize = static_cast<IndexType>(dim.y);

	for (IndexType i = 0; i < colSize - 1; ++i)
	{
		for (IndexType j = 0; j < rowSize - 1; ++j)
		{
			IndexType value = vertexOffset + j + i * rowSize;

			buffer->setIndex(value, k);
			buffer->setIndex(value + rowSize, k+1);
			buffer->setIndex(value + 1, k+2);
			buffer->setIndex(value + rowSize, k+3);
			buffer->setIndex(value + rowSize + 1, k+4);
			buffer->setIndex(value + 1, k+5);
			k += 6;
		} 
	}
	
	return k;
}

void Primitives::calculateNormals(VertexArray::Pointer data, const IndexArray::Pointer& buffer, size_t first, size_t last)
{
	VertexDataChunk posChunk = data->chunk(Usage_Position);
	VertexDataChunk nrmChunk = data->chunk(Usage_Normal);
	if (!posChunk.valid() || (posChunk->type() != Type_Vec3) || !nrmChunk.valid() || (nrmChunk->type() != Type_Vec3)) 
	{
		std::cout << "Primitives::calculateNormals - data is invalid." << std::endl;
		return;
	}
	
	RawDataAcessor<vec3> pos = posChunk.accessData<vec3>(0);
	RawDataAcessor<vec3> nrm = nrmChunk.accessData<vec3>(0);
	for (IndexArray::PrimitiveIterator i = buffer->primitive(first), e = buffer->primitive(last); i != e; ++i)
	{
		const IndexArray::Primitive& p = (*i);
		vec3& v0 = pos[p[0]];
		vec3& v1 = pos[p[1]];
		vec3& v2 = pos[p[2]];
		triangle t(v0, v1, v2);
		vec3 n = plane(t).normal() * t.square();
		nrm[p[0]] += n;
		nrm[p[1]] += n;
		nrm[p[2]] += n;
	}

	for (IndexArray::PrimitiveIterator i = buffer->primitive(first), e = buffer->primitive(last); i != e; ++i)
	{
		const IndexArray::Primitive& p = (*i);
		for (size_t k = 0; k < 3; ++k)
			nrm[p[k]].normalize();
	}
}

void Primitives::calculateTangents(VertexArray::Pointer data, const IndexArray::Pointer& buffer, size_t first, size_t last)
{
	VertexDataChunk posChunk = data->chunk(Usage_Position);
	VertexDataChunk nrmChunk = data->chunk(Usage_Normal);
	VertexDataChunk uvChunk = data->chunk(Usage_TexCoord0);
	VertexDataChunk tanChunk = data->chunk(Usage_Tangent);
	if (!posChunk.valid() || (posChunk->type() != Type_Vec3) || 
		!nrmChunk.valid() || (nrmChunk->type() != Type_Vec3) ||
		!tanChunk.valid() || (tanChunk->type() != Type_Vec3) ||
		!uvChunk.valid() || (uvChunk->type() != Type_Vec2)) 
	{
		std::cout << "Primitives::calculateTangents - data is invalid." << std::endl;
		return;
	}

	DataStorage<vec3> tan1(data->size());
	DataStorage<vec3> tan2(data->size());

	RawDataAcessor<vec3> pos = posChunk.accessData<vec3>(0);
	RawDataAcessor<vec3> nrm = nrmChunk.accessData<vec3>(0);
	RawDataAcessor<vec3> tan = tanChunk.accessData<vec3>(0);
	RawDataAcessor<vec2> uv = uvChunk.accessData<vec2>(0);
	
	for (IndexArray::PrimitiveIterator i = buffer->primitive(first), e = buffer->primitive(last); i != e; ++i)
	{
		IndexArray::Primitive& p = (*i);
		
		vec3& v1 = pos[p[0]];
		vec3& v2 = pos[p[1]];
		vec3& v3 = pos[p[2]];
		vec2& w1 = uv[p[0]];
		vec2& w2 = uv[p[1]];
		vec2& w3 = uv[p[2]];

		float x1 = v2.x - v1.x; 
		float x2 = v3.x - v1.x; 
		float y1 = v2.y - v1.y; 
		float y2 = v3.y - v1.y; 
		float z1 = v2.z - v1.z; 
		float z2 = v3.z - v1.z; 
		float s1 = w2.x - w1.x; 
		float s2 = w3.x - w1.x; 
		float t1 = w2.y - w1.y; 
		float t2 = w3.y - w1.y; 
		float r = 1.0f / (s1 * t2 - s2 * t1); 

		vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
		vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r); 

		tan1[p[0]] += sdir; 
		tan1[p[1]] += sdir; 
		tan1[p[2]] += sdir;
		tan2[p[0]] += tdir; 
		tan2[p[1]] += tdir; 
		tan2[p[2]] += tdir; 
	} 

	for (IndexArray::PrimitiveIterator i = buffer->primitive(first), e = buffer->primitive(last); i != e; ++i)
	{
		const IndexArray::Primitive& p = (*i);
		for (size_t k = 0; k < 3; ++k)
		{
			vec3& n = nrm[p[k]];
			vec3& t = tan1[p[k]]; 
			float value = dot(cross(n, t), tan2[p[k]]);
			tan[p[k]] = normalize(t - n * dot(n, t)) * sign(value);
		}
	}
}

void Primitives::smoothTangents(VertexArray::Pointer data, const IndexArray::Pointer&, size_t first, size_t last)
{
	VertexDataChunk posChunk = data->chunk(Usage_Position);
	VertexDataChunk tanChunk = data->chunk(Usage_Tangent);
	if (!posChunk.valid() || (posChunk->type() != Type_Vec3) || !tanChunk.valid() || (tanChunk->type() != Type_Vec3))
	{
		std::cout << "Primitives::smoothTangents - data is invalid." << std::endl;
		return;
	}

	size_t len = last - first;
	DataStorage<vec3> tanSmooth(data->size());
	RawDataAcessor<vec3> pos = posChunk.accessData<vec3>(first);
	RawDataAcessor<vec3> tan = tanChunk.accessData<vec3>(first);
	for (size_t p = 0; p < len; ++p)
	{
		tanSmooth[p] = tan[p];
		for (size_t i = p + 1; i < len; ++i)
		{
			if ((pos[p] - pos[i]).dotSelf() <= 1.0e-3)
				tanSmooth[p] += tan[i];
		}
	}

	for (size_t i = 0; i < len; ++i)
		tan[i] = normalize(tanSmooth[i]);
}