vMieColor      = vec3(0.0);
vRayleighColor = vec3(0.0);

for(int i = 0; i < nPlanetSamples; i++)
{
 float fHeight   = length(v3SamplePoint);
 float fDepth    = exp(vScale.z * (vRadius.x - fHeight));
 float fScatter  = fDepth * fTemp - fCameraOffset;
 vec3 vAttenuate = exp(-fScatter * (vInvWaveLength * vK.x + vK.z));
 vMieColor      += vAttenuate;
 vRayleighColor += vAttenuate * (fDepth * fScaledLength);
 v3SamplePoint  += v3SampleRay;
}

vRayleighColor *= vInvWaveLength * vK.y + vK.w;
vMieColor      /= fPlanetSamples;
