#include "RVector.h"

bool RV3Colider::ColisionSphereToPlane(const Sphere &sphere, const Plane &plane, RVector3 *coliPos)
{
	//平面と球の中心との距離を求める
	float dist = dot(sphere.center, plane.normal) - plane.distance;
	//距離の絶対値が半径より大きければあたってない
	if (fabsf(dist) > sphere.rad) { return false; }
	//あたっている場合は疑似交点を計算
	*coliPos = -dist * plane.normal + sphere.center;
	//衝突
	return true;
}
