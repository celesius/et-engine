float fSampleLength = fFar / fSamples;
float fScaledLength = fSampleLength * vScale.x;
vec3 v3SampleRay    = v3Ray * fSampleLength;
vec3 v3SamplePoint  = v3Start + v3SampleRay * 0.5;