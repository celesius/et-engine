float scale(float fCos)
{
 float x = 1.0 - fCos;
 return vScale.y * exp(x*(0.459 + x*(3.83 + x*(x*5.25 - 6.80))));
}
