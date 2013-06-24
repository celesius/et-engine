uniform mat4 mInverseMVPMatrix;

etVertexIn vec2 Vertex;
etVertexOut vec2 vVertex;

void main()
{
	vVertex = Vertex;
	gl_Position = vec4(Vertex, 0.0, 1.0);
}