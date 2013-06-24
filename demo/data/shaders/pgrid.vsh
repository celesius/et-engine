uniform sampler2D cloudsTexture;
uniform mat4 mModelViewProjection;
uniform mat4 mInverseMVPMatrix;

etVertexIn vec2 Vertex;
etVertexOut vec4 vVertex;

void main()
{
	vec4 start = mInverseMVPMatrix * vec4(Vertex, -1.0, 1.0);
	vec4 dir = mInverseMVPMatrix * vec4(Vertex, 1.0, 1.0) - start;
	
	vec4 homoPos = start - (start.y / dir.y) * dir;
	vec3 worldPos = homoPos.xyz / homoPos.w;

	float scale0 = 10.0;
	float scale1 = 3.0;
	float scale2 = 1.0;

	vVertex = scale0 * etTexture2D(cloudsTexture, 0.005 * worldPos.xz) +
		scale1 * etTexture2D(cloudsTexture, 0.01 * worldPos.xz) +
		scale2 * etTexture2D(cloudsTexture, 0.02 * worldPos.xz);

	homoPos.y += vVertex.x * homoPos.w;

	vVertex = vec4(0.1) + (0.9 / (scale0 + scale1 + scale2)) * vVertex;
	
	gl_Position = mModelViewProjection * homoPos;
}