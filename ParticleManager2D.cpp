#include "ParticleManager2D.h"

using namespace std;

namespace pm2d_utl {
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
}

using namespace pm2d_utl;

ParticleManager2D::ParticleManager2D()
{
}

ParticleManager2D::~ParticleManager2D()
{
}

void ParticleManager2D::Initialize(UINT graphHandle)
{
	_psprite.Create(graphHandle);
}

void ParticleManager2D::Add(ParticleGrainState2D& pgState2d)
{
}

void ParticleManager2D::Update()
{
	//�����؂�폜
	std::erase_if(_grains, [](std::unique_ptr<Grain2d> &g) {return g->nowFrame > g->endFrame; });

	//�p�����[�^�[�ɂ��X�V
	for (auto& e : _grains) {

		//���S���W
		e->pos += e->vel;
		e->vel += e->acc;

		//�⊮���[�g
		float rate = e->nowFrame / e->endFrame;

		//�F���`���
		e->color = e->s_color + (e->e_color - e->s_color) * rate;

		//�X�P�[�����O���`���
		e->scale = e->s_scale + (e->e_scale - e->s_scale) * rate;

		//��]�⊮
		e->rot = e->rot + (e->e_rotation - e->s_rotation) * rate;

		//�����J�E���g
		e->nowFrame++;
	}

}

void ParticleManager2D::Draw()
{



}

void ParticleManager2D::Prototype_Set(ParticlePrototype2D* ins)
{
	_prototype.reset(ins);
}

void ParticleManager2D::Prototype_Add()
{
	std::unique_ptr<ParticlePrototype2D> newp(_prototype.get()->clone());
	newp->Init();
	_pplist.emplace_back(std::move(newp));
}

void ParticleManager2D::Prototype_Update()
{
	erase_if(_pplist, [](std::unique_ptr<ParticlePrototype2D>& p) {return p->nowFrame >= p->endFrame; });

	for (auto& p : _pplist) {
		p->Update();
		p->nowFrame++;
	}
}

void ParticleManager2D::Prototype_Draw()
{
	if (_pplist.size() == 0) { return; }

	for (auto& p : _pplist) {
		//�X�P�[�����O�ɂ����W�̈ړ�
		float x1 = p->pos.x - (p->drawsize.x / 2) * (1.0f * p->scale);
		float y1 = p->pos.y - (p->drawsize.y / 2) * (1.0f * p->scale);
		float x2 = p->pos.x + (p->drawsize.x / 2) * (1.0f * p->scale);
		float y2 = p->pos.y + (p->drawsize.y / 2) * (1.0f * p->scale);

		Sprite::SetSpriteColorParam(p->color.x, p->color.y, p->color.z, p->color.w);

		//�p�����[�^�[�ɍ��킹���`��
		_psprite.DrawRotaSprite(x1, y1, x2, y2, p->rot);
	}

	_psprite.Draw();

	Sprite::SetSpriteColorParam(1.0f, 1.0f, 1.0f, 1.0f);
}
