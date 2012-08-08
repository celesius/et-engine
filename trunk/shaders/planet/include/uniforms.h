uniform mat4 mModelViewProjection;
uniform vec3 vCamera;
uniform vec3 vPrimaryLight;
uniform vec3 vInvWaveLength;
uniform vec4 vRadius;
uniform vec4 vK;
uniform vec3 vScale;

const int nAthmoSamples    = 3;
const int nPlanetSamples   = 2;
const float fAthmoSamples  = float(nAthmoSamples);
const float fPlanetSamples = float(nPlanetSamples); 