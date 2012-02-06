vec2 Blinn(vec3 _normal, vec3 _light, vec3 _view, float roughness_val)
{
 float Rs = 0.0;
 float NdotL = max( dot( _normal, _light ), 0.0 );
 
 if (roughness_val > 0.0)
  {
   vec3 half_vec = normalize( _view + _light );
   float NdotH   = max( 0.0, dot( _normal, half_vec ) );
         Rs      = NdotL * pow( NdotH, 7.5 / roughness_val );
  }
 return vec2( NdotL, Rs );
}
