uniform sampler2D cloudsTexture;

etFragmentIn vec3 vViewWS;
etFragmentIn vec3 vLightWS;
etFragmentIn vec3 vNormalWS;
etFragmentIn vec2 vTextureCoord0;
etFragmentIn vec2 vTextureCoord1;

#include "cooktorrance.h"

void main()
{
	vec3 n = normalize(vNormalWS);
	vec3 l = normalize(vLightWS);
	vec3 v = normalize(vViewWS);

	vec4 c1 = etTexture2D(cloudsTexture, vTextureCoord0);
	vec4 c2 = etTexture2D(cloudsTexture, vTextureCoord1);

	vec2 light = CookTorrance(n, l, v, 1.0 - c2.y);

	vec4 dc = vec4(1.0, 0.75, 0.5, 1.0);
	vec4 sc = vec4(0.0625, 0.125, 0.6666, 1.0);

	etFragmentOut = (0.5 + 0.5 * c1.x) * dc * light.x + sc * (c2.y + light.y);
}