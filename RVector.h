#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//XMFLOAT3を演算子オーバーロードさせたRVector3型
class RVector3 : public XMFLOAT3
{
public:
	//コンストラクタ
	RVector3(float x, float y, float z) : XMFLOAT3(x, y, z) {};
	RVector3() :XMFLOAT3(0, 0, 0) {};

	RVector3 operator+(const RVector3 &other)const { return RVector3(x + other.x, y + other.y, z + other.z); }
	RVector3 operator-(const RVector3 &other)const { return RVector3(x - other.x, y - other.y, z - other.z); }
	RVector3 operator*(const RVector3 &other)const { return RVector3(x * other.x, y * other.y, z * other.z); }
	RVector3 operator/(const RVector3 &other)const { return RVector3(x / other.x, y / other.y, z / other.z); }
	RVector3 &operator+=(const RVector3 &other) { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
	RVector3 &operator-=(const RVector3 &other) { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
	RVector3 &operator*=(const RVector3 &other) { this->x *= other.x; this->y *= other.y; this->z *= other.z; return *this; }
	RVector3 &operator/=(const RVector3 &other) { this->x /= other.x; this->y /= other.y; this->z /= other.z; return *this; }

	bool operator==(const RVector3 &other) { return this->x == other.x && this->y == other.y && this->z == other.z; }
	bool operator!=(const RVector3 &other) { return !(*this == other); }

	RVector3 operator*(const float &other) { return RVector3(x * other, y * other, z * other); }
	RVector3 operator/(const float &other) { return RVector3(x / other, y / other, z / other); }
	RVector3 operator*=(const float &other) { this->x *= other; this->y *= other; return *this; }
	RVector3 operator/=(const float &other) { this->x /= other; this->y /= other; return *this; }

	float length() { return sqrtf(x * x + y * y + z * z); }
	RVector3 norm() { return RVector3(x / length(), y / length(), z / length()); }
	float distance(const RVector3 &v) { return sqrtf(pow(v.x - x, 2) + pow(v.y - y, 2) + pow(v.z - z, 2)); }

	void zero() { x = 0, y = 0, z = 0; }

	RVector3 operator-() const { return RVector3(-x, -y, -z); }
};

inline float dot(const RVector3 &a, const RVector3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline RVector3 cross(const RVector3 &a, const RVector3 &b) { return RVector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

inline float distance(const RVector3 &v1,const RVector3 &v2){return sqrtf(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));}

inline RVector3 operator*(const float &sum, const RVector3 &sum2) { return RVector3(sum2.x * sum, sum2.y * sum, sum2.z * sum); }
inline RVector3 operator/(const float &sum, const RVector3 &sum2) { return RVector3(sum2.x / sum, sum2.y / sum, sum2.z / sum); }

//RVector3型を使用した衝突判定プリミティブ
namespace RV3Colider {

	///軸並行バウンディングボックス AABB
	struct Rv3AABB {

		RVector3 min;	//判定サイズ
		RVector3 max;	//判定サイズ
		RVector3 oldPos;//1F前の座標

		//コンストラクタ、デストラクタ
		Rv3AABB()	= default;
		~Rv3AABB()	= default;

		//値指定コンストラクタ
		Rv3AABB(RVector3 min, RVector3 max, RVector3 pos) {
			this->min = min;
			this->max = max;
			this->oldPos = pos;
		}

		inline void Set(RVector3 pos, RVector3 min, RVector3 max) {
			this->min = min;
			this->max = max;
			this->oldPos = pos;
		}

		inline void Update(RVector3 actpos) {
			RVector3 diff = actpos - oldPos;
			min += diff;
			max += diff;
			oldPos = actpos;
		}
	};

	/// <summary>
	/// AABB同士の判定
	/// </summary>
	/// <param name="box1">ボックス1</param>
	/// <param name="box2">ボックス2</param>
	/// <returns>衝突判定</returns>
	inline bool ColisionAABB(const Rv3AABB &box1, const Rv3AABB &box2) {
		//非衝突
		if (box1.min.x > box2.max.x) { return false; }
		if (box1.max.x < box2.min.x) { return false; }
		if (box1.min.y > box2.max.y) { return false; }
		if (box1.max.y < box2.min.y) { return false; }
		if (box1.min.z > box2.max.z) { return false; }
		if (box1.max.z < box2.min.z) { return false; }
		//衝突
		return true;
	}

	//球
	struct Sphere {
		//中心座標
		RVector3 center;
		//半径
		float rad;
		//デフォルトコンストラクタ
		Sphere() {
			center = RVector3(0, 0, 0);
			rad = 0;
		}
		//値指定コンストラクタ
		Sphere(RVector3 pos, float r) {
			center = pos;
			rad = r;
		}
	};

	/// <summary>
	/// 球同士の判定
	/// </summary>
	/// <param name="a">球1</param>
	/// <param name="b">球2</param>
	/// <returns>衝突判定</returns>
	inline bool Colision2Sphere(Sphere a, Sphere b) {
		if (a.rad + b.rad < distance(a.center, b.center)) {
			return false;
		}
		//衝突
		return true;
	}

	//平面
	struct Plane
	{
		//法線ベクトル
		RVector3 normal;
		//原点からの距離
		float distance;
		//デフォルト
		Plane() {
			normal.zero();
			distance = 0;
		}
		//値指定
		Plane(RVector3 n, float d) {
			normal = n;
			distance = d;
		}
	};

	/// <summary>
	/// 球と平面の衝突判定
	/// </summary>
	/// <param name="sphere">球体プリミティブ</param>
	/// <param name="plane">平面プリミティブ</param>
	/// <param name="coliPos">衝突点を返す変数</param>
	/// <returns>衝突判定</returns>
	bool ColisionSphereToPlane(const Sphere &sphere, const Plane &plane, RVector3 *coliPos = nullptr);

	//三角形
	struct Triangle {
		//頂点座標
		RVector3 p0, p1, p2;
		//法線
		RVector3 normal;

	};

};
