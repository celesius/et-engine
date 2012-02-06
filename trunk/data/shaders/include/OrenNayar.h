float OrenNayar(vec3 _normal, vec3 _light, vec3 _view, float roughness_val)
{
 const float PI = 3.14159265359;

 vec3 n = _normal;
 vec3 l = _light;
 vec3 v = _view;

 float VdotN = dot(_view, _normal);
 float LdotN = dot(_light, _normal);

 float alpha = max(acos(VdotN), acos(LdotN));
 float beta = min(acos(VdotN), acos(LdotN));
 float gamma = dot(v - n * VdotN, l - n * LdotN);
 float rough_sq = roughness_val * roughness_val;

 float C1 = 1.0 - 0.50 * rough_sq / (rough_sq + 0.33);
 float C2 = 0.45 * rough_sq / (rough_sq + 0.09);

 C2 *= gamma >= 0.0 ? sin(alpha) : sin(alpha) - pow((2.0 * beta) / PI, 3.0);

 float fC3mul = (4.0 * alpha * beta) / (PI * PI);
 float C3 = 0.125 * (rough_sq / (rough_sq + 0.09)) * fC3mul * fC3mul;
 float A = gamma * C2 * tan(beta);
 float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);

 return max(max(0.0, dot(n, l)) * (C1 + A + B), 0.0);
}
