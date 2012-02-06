float getLinearDistance(float depth, vec2 clipPlane)
{
 return clipPlane.x * clipPlane.y / (clipPlane.y - depth * (clipPlane.y - clipPlane.x));
}