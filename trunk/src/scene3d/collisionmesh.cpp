#include <et/scene3d/collisionmesh.h>

using namespace et;
using namespace et::s3d;

CollisionMesh::CollisionMesh(const std::string& name, Element* parent) : Mesh(name, parent),
	_cachedFinalTransformScale(0.0f), _inverseTransformValid(false)
{

}

CollisionMesh::CollisionMesh(const std::string& name, const VertexArrayObject& ib, const Material& material,
	size_t startIndex, size_t numIndexes, Element* parent) : Mesh(name, ib, material, startIndex, numIndexes, parent),
	_data(numIndexes / 3), _cachedFinalTransformScale(0.0f), _inverseTransformValid(false)
{

}

void CollisionMesh::setNumIndexes(size_t num)
{
	Mesh::setNumIndexes(num);
	_data.fitToSize(num / 3);
}

void CollisionMesh::fillCollisionData(VertexArrayRef v, IndexArrayRef ind)
{
	RawDataAcessor<vec3> pos = v->chunk(Usage_Position).accessData<vec3>(0);

	_data.setOffset(0);
	size_t index = 0;
	vec3 minOffset;
	vec3 maxOffset;
	float distance = 0.0f;
	size_t iStart = startIndex() / 3;
	size_t iEnd = iStart + numIndexes() / 3;
	for (IndexArray::PrimitiveIterator i = ind->primitive(iStart) , e = ind->primitive(iEnd); i != e; ++i)
	{
		IndexArray::Primitive& p = *i;
		const vec3& p0 = pos[p[0]];
		const vec3& p1 = pos[p[1]];
		const vec3& p2 = pos[p[2]];
		_data.push_back(triangle(p0, p1, p2));

		if (index == 0)
		{
			minOffset = minv(p0, minv(p1, p2));
			maxOffset = maxv(p0, maxv(p1, p2));
			distance = etMax(p0.length(), etMax(p1.length(), p2.length()));
		}
		else
		{
			minOffset = minv(minOffset, p0);
			minOffset = minv(minOffset, p1);
			minOffset = minv(minOffset, p2);
			maxOffset = maxv(maxOffset, p0);
			maxOffset = maxv(maxOffset, p1);
			maxOffset = maxv(maxOffset, p2);
			distance = etMax(p0.length(), etMax(p1.length(), p2.length()));
		}
		++index;
	}

	_size = maxv(0.5f * (maxOffset - minOffset), vec3(0.0001f));
	_center = 0.5f * (maxOffset + minOffset);
	_radius = distance;
}

CollisionMesh* CollisionMesh::duplicate()
{
	CollisionMesh* result = new CollisionMesh(name(), vertexArrayObject(), material(), startIndex(), numIndexes(), parent());
	result->_size = _size;
	result->_center = _center;
	result->_radius = _radius;
	result->_data = _data;
	return result;
}

Sphere CollisionMesh::sphere() 
{
	const vec3& s = scale();
	return Sphere(finalTransform() * _center, etMax(s.x, etMax(s.y, s.z)) * _radius);
}

AABB CollisionMesh::aabb()
{
	mat4 ft = finalTransform();
	return AABB(ft * _center, absv(ft.rotationMultiply(_size)));
}

OBB CollisionMesh::obb()
{
	mat4 ft = finalTransform();
	mat3 r = ft.mat3();
	vec3 s = removeMatrixScale(r);
	return OBB(ft * _center, s * _size, r);
}

void CollisionMesh::serialize(std::ostream& stream)
{
	serializeFloat(stream, _radius);
	serializeVector(stream, _size);
	serializeVector(stream, _center);
	serializeInt(stream, _data.size());
	stream.write(_data.binary(), _data.dataSize());
	Mesh::serialize(stream);
}

void CollisionMesh::deserialize(std::istream& stream, ElementFactory* factory)
{
	_radius = deserializeFloat(stream);
	_size = deserializeVector<vec3>(stream);
	_center = deserializeVector<vec3>(stream);
	_data.resize(deserializeInt(stream));
	stream.read(_data.binary(), _data.dataSize());
	Mesh::deserialize(stream, factory);
}

mat4 CollisionMesh::finalTransform()
{
	if (!transformValid())
		_inverseTransformValid = false;

	return Element::finalTransform();
}

mat4 CollisionMesh::finalTransformInverse()
{
	if (!_inverseTransformValid)
		buildInverseTransform();

	return _cachedInverseTransform;
}

float CollisionMesh::finalTransformScale()
{
	if (!_inverseTransformValid)
		buildInverseTransform();

	return _cachedFinalTransformScale;
}

void CollisionMesh::buildInverseTransform()
{
	_cachedInverseTransform = Element::finalTransform().inverse();
	_cachedFinalTransformScale = 1.0f / powf(_cachedInverseTransform.mat3().determinant(), 1.0f / 3.0f);
	_inverseTransformValid = true;
}