const float fPolar = 0.0020765;

float fSinTheta = sin(Vertex.x);
float fRadius   = vRadius.z * (1.0 - fPolar * fSinTheta);

vec4 vVertex;
vVertex.x = fRadius * fSinTheta * cos(Vertex.y);
vVertex.y = fRadius             * cos(Vertex.x);
vVertex.z = fRadius * fSinTheta * sin(Vertex.y);
vVertex.w = 1.0;
