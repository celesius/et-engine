vRayleighColor = vec3(0.0);
for(int i = 0; i < nAthmoSamples; i++)
{
 float fHeight      = length(v3SamplePoint);
 float fDepth       = exp(vScale.z * (vRadius.x - fHeight));
 float fLightAngle  = dot(vPrimaryLight, v3SamplePoint) / fHeight;
 float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
 float fScatter     = fStartOffset + fDepth * (scale(fLightAngle) - scale(fCameraAngle));
 vec3 v3Attenuate   = exp(-fScatter * (vInvWaveLength * vK.x + vK.z));
 vRayleighColor    += v3Attenuate * (fDepth * fScaledLength);
 v3SamplePoint     += v3SampleRay;
}
vMieColor       = vRayleighColor * vK.w;
vRayleighColor *= vInvWaveLength * vK.y;
