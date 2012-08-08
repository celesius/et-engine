inline float RayleighPhase(float fCos2)
{
 return 0.75 + 0.75 * fCos2;
}

inline float MiePhase(float fCos, float fCos2, float g, float gexp)
{
 float fD = 1.0 + g * (g - 2.0 * fCos);

 return (1.0 + fCos2) * gexp / fD / sqrt(fD); 
}
