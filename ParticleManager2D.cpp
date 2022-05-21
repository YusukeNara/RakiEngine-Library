#include "ParticleManager2D.h"

const DirectX::XMFLOAT4 operator+(const DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs) {
	XMFLOAT4 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	result.w = lhs.w + rhs.w;
	return result;
}

static void operator+=(DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	lhs.w += rhs.w;
}

const DirectX::XMFLOAT4 operator-(DirectX::XMFLOAT4& lhs, const DirectX::XMFLOAT4& rhs) {
	XMFLOAT4 result;
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	result.z = lhs.z - rhs.z;
	result.w = lhs.w - rhs.w;
	return result;
}

const DirectX::XMFLOAT4 operator/(const DirectX::XMFLOAT4& lhs, const float a) {
	XMFLOAT4 result;
	result.x = lhs.x / a;
	result.y = lhs.y / a;
	result.z = lhs.z / a;
	result.w = lhs.w / a;
	return result;
}

const DirectX::XMFLOAT4 operator*(const DirectX::XMFLOAT4& lhs, const float a) {
	XMFLOAT4 result;
	result.x = lhs.x * a;
	result.y = lhs.y * a;
	result.z = lhs.z * a;
	result.w = lhs.w * a;
	return result;
}

ParticleManager2D::ParticleManager2D()
{
}

ParticleManager2D::~ParticleManager2D()
{
}

void ParticleManager2D::Initialize(UINT graphHandle)
{
}

void ParticleManager2D::Add(ParticleGrainState2D& pgState2d)
{
}

void ParticleManager2D::Update()
{
	//寿命切れ削除
	std::erase_if(_pplist, [](Grain2d* g) {return g->nowFrame > g->endFrame; });

	//パラメーターによる更新
	for (auto& e : _pplist) {

		//中心座標
		e->pos += e->vel;
		e->vel += e->acc;

		//補完レート
		float rate = e->nowFrame / e->endFrame;

		//色線形補間
		e->color = e->s_color + (e->e_color - e->s_color) * rate;

		//スケーリング線形補間
		e->scale = e->s_scale + (e->e_scale - e->s_scale) * rate;

		//回転補完
		e->rot = e->rot + (e->e_rotation - e->s_rotation) * rate;

	}


}

void ParticleManager2D::Draw()
{
}

void ParticleManager2D::Prototype_Set(ParticlePrototype2D* ins)
{
}

void ParticleManager2D::Prototype_Add()
{
}

void ParticleManager2D::Prototype_Update()
{
}

void ParticleManager2D::Prototype_Draw()
{



}
