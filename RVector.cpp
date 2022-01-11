#include "RVector.h"

bool RV3Colider::ColisionSphereToPlane(const Sphere &sphere, const Plane &plane, RVector3 *coliPos)
{
	//•½–Ê‚Æ‹…‚Ì’†S‚Æ‚Ì‹——£‚ğ‹‚ß‚é
	float dist = dot(sphere.center, plane.normal) - plane.distance;
	//‹——£‚Ìâ‘Î’l‚ª”¼Œa‚æ‚è‘å‚«‚¯‚ê‚Î‚ ‚½‚Á‚Ä‚È‚¢
	if (fabsf(dist) > sphere.rad) { return false; }
	//‚ ‚½‚Á‚Ä‚¢‚éê‡‚Í‹^—Œğ“_‚ğŒvZ
	*coliPos = -dist * plane.normal + sphere.center;
	//Õ“Ë
	return true;
}
