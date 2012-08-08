vViewWS    = vVertex.xyz - vCamera;
vec3 v3Ray = vViewWS;
float fFar = length(v3Ray);
v3Ray     /= fFar;
