float fSinTheta = sin(Vertex.x);
float fCosTheta = cos(Vertex.x);

const float fPolar = 0.0020765;
float sinx      = vAngle.x;
float siny      = vAngle.y;
float cosx      = cos(vAngle.z);
float cosy      = cos(vAngle.w);

float fRadius = vRadius.x * (1.0 - fPolar * fSinTheta);

vec3 vVert;
vVert.x = fRadius * cos(Vertex.y) * fSinTheta;
vVert.y = fRadius * fCosTheta;
vVert.z = fRadius * sin(Vertex.y) * fSinTheta;

mat3 MXY;
MXY[0][0] =  cosy      ;MXY[0][1] =  0.0 ;MXY[0][2] =  siny;
MXY[1][0] =  sinx*siny ;MXY[1][1] = cosx ;MXY[1][2] = -sinx*cosy;
MXY[2][0] = -cosx*siny ;MXY[2][1] = sinx ;MXY[2][2] =  cosx*cosy;
vec4 vVertex = vec4(MXY * vVert, 1.0);
