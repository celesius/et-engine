#define LN_10 2.3025850929940456840179914546844

vec3 log10(vec3 value)
{
 return log(value) / LN_10;
}

vec3 convertRGBtoXYZ(vec3 rgb)
{
 float r = rgb.x;
 float g = rgb.y;
 float b = rgb.z;

 if (r > 0.04045)
  r = pow((r + 0.055) / 1.055, 2.4);
 else
  r = r / 12.92;

 if ( g > 0.04045)
  g = pow((g + 0.055) / 1.055, 2.4); 
 else
  g = g / 12.92;

 if (b > 0.04045)
  b = pow((b + 0.055) / 1.055, 2.4);
 else 
  b = b / 12.92;

 vec3 xyz;
 xyz.x = r * 0.4124 + g * 0.3576 + b * 0.1805;
 xyz.y = r * 0.2126 + g * 0.7152 + b * 0.0722;
 xyz.z = r * 0.0193 + g * 0.1192 + b * 0.9505;
 return xyz;
}

vec3 convertRGBtoLAB(vec3 rgb)
{ 
 vec3 xyz = convertRGBtoXYZ(rgb);
 float x = xyz.x;
 float y = xyz.y;
 float z = xyz.z;

 float REF_X =  95.047; // Observer= 2°, Illuminant= D65
 float REF_Y = 100.000; 
 float REF_Z = 108.883; 
		
 if ( x > 0.008856 ) { x = Math.pow( x , 1/3 ); }
 else { x = ( 7.787 * x ) + ( 16/116 ); }

 if ( y > 0.008856 ) { y = Math.pow( y , 1/3 ); }
 else { y = ( 7.787 * y ) + ( 16/116 ); }

 if ( z > 0.008856 ) { z = Math.pow( z , 1/3 ); }
 else { z = ( 7.787 * z ) + ( 16/116 ); }
 
 vec3 lab;
 lab.x = (116 * y) - 16;
 lab.y = 500 * (x - y);
 lab.z = 200 * (y - z);
 return lab;
}