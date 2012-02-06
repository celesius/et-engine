vec3 spherical_1(float a, float b)
{
 vec3 result;
 float cosA = cos(a);
 result.x = cosA * cos(b);
 result.y = sin(a);
 result.z = cosA * sin(b);
 return result;
}

vec3 spherical_2(float a, float b)
{
 vec3 result;
 float cosA = cos(a);
 result.x = cosA * cos(b);
 result.y = cosA * sin(b);
 result.z = sin(a);
 return result;
}

vec4 sampleCubemap(samplerCube cubemap_texture, vec3 v0, float offsetScale)
{
 vec4 color = texture(cubemap_texture, v0);

 float c = 0.01745329251994329576923690768489 * offsetScale;
 const int numSamples = 8;

 if ( abs(v0.y) >= 0.9 )
 {
  float a = asin(v0.z);
  float b = atan(v0.y, v0.x);

  for (int i = 0; i < numSamples; ++i)
  {
   float dc = i * c;
   color += texture(cubemap_texture, spherical_2(a, b + dc));
   color += texture(cubemap_texture, spherical_2(a, b - dc));
   color += texture(cubemap_texture, spherical_2(a + dc, b));
   color += texture(cubemap_texture, spherical_2(a - dc, b));
   color += texture(cubemap_texture, spherical_2(a + dc, b + dc));
   color += texture(cubemap_texture, spherical_2(a + dc, b - dc));
   color += texture(cubemap_texture, spherical_2(a - dc, b + dc));
   color += texture(cubemap_texture, spherical_2(a - dc, b - dc));
  }
 }
 else
 {
  float a = asin(v0.y);
  float b = atan(v0.z, v0.x);

  for (int i = 0; i < numSamples; ++i)
  {
   float dc = i * c;
   color += texture(cubemap_texture, spherical_1(a, b + dc));
   color += texture(cubemap_texture, spherical_1(a, b - dc));
   color += texture(cubemap_texture, spherical_1(a + dc, b));
   color += texture(cubemap_texture, spherical_1(a - dc, b));
   color += texture(cubemap_texture, spherical_1(a + dc, b + dc));
   color += texture(cubemap_texture, spherical_1(a + dc, b - dc));
   color += texture(cubemap_texture, spherical_1(a - dc, b + dc));
   color += texture(cubemap_texture, spherical_1(a - dc, b - dc));
  }
 }

 return color / (9.0 * numSamples);
}