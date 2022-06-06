#include "NY_Object3D.h"

#include "NY_Camera.h"
#include "TexManager.h"
#include "Raki_DX12B.h"
#include "NY_Object3DMgr.h"

//----- NY_Object3D -----//

void Object3d::InitObject3D(ID3D12Device *dev)
{

	HRESULT result;
	const auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);
	
	//�萔�o�b�t�@b0����
	result =  dev->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0)
	);

	//�萔�o�b�t�@b1����
	result = dev->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1)
	);

}

void Object3d::SetLoadedModelData(Model3D *loadedModel)
{
	//model = loadedModel;

	isDirty = true;
}

void Object3d::SetWorldMatrix(XMMATRIX matWorld)
{
	//�ϊ��s���ݒ�
	this->matWorld = matWorld;
	//�_�[�e�B�t���O�L����
	isDirty = true;
}

void Object3d::SetAffineParam(RVector3 scale, RVector3 rot, RVector3 trans)
{
	//�A�t�B���ϊ������r���A�l���قȂ�ꍇ�X�V����
	if (this->scale != scale) {
		this->scale = scale;
		isDirty = true;
	}
	if (this->rotation != rot) {
		this->rotation = rot;
		isDirty = true;
	}
	if (this->position != trans) {
		this->position = trans;
		isDirty = true;
	}

	//�_�[�e�B�t���O���L���̂Ƃ�
	if (isDirty == true) {
		//�����ɉ����ăp�����[�^�X�V
		if (isBillBoard != true) {
			UpdateObject3D();
		}
		else {
			UpdateBillBoard3D();
		}
	}
}

void Object3d::SetAffineParamScale(RVector3 scale)
{
	if (this->scale != scale) {
		this->scale = scale;
		isDirty = true;
	}

	if (isDirty == true) {
		if (isBillBoard != true) {
			UpdateObject3D();
		}
		else {
			UpdateBillBoard3D();
		}
	}
}

void Object3d::SetAffineParamRotate(RVector3 rot)
{
	if (this->rotation != rot) {
		this->rotation = rot;
		isDirty = true;
	}

	if (isDirty == true) {
		if (isBillBoard != true) {
			UpdateObject3D();
		}
		else {
			UpdateBillBoard3D();
		}
	}
}

void Object3d::SetAffineParamTranslate(RVector3 trans)
{
	if (this->position != trans) {
		this->position = trans;
		isDirty = true;
	}

	if (isDirty == true) {
		if (isBillBoard != true) {
			UpdateObject3D();
		}
		else {
			UpdateBillBoard3D();
		}
	}
}

//void Object3d::UpdateObject3D(Object3d *obj, XMMATRIX &matview)
//{
//	XMMATRIX matScale, matRot, matTrans;
//
//	// �X�P�[���A��]�A���s�s��̌v�Z
//
//	matScale = XMMatrixScaling(obj->scale.x, obj->scale.y, obj->scale.z);
//
//	matRot = XMMatrixIdentity();
//	matRot *= XMMatrixRotationZ(XMConvertToRadians(obj->rotation.z));
//	matRot *= XMMatrixRotationX(XMConvertToRadians(obj->rotation.x));
//	matRot *= XMMatrixRotationY(XMConvertToRadians(obj->rotation.y));
//
//	matTrans = XMMatrixTranslation(obj->position.x, obj->position.y, obj->position.z);
//
//	obj->matWorld = XMMatrixIdentity();
//	obj->matWorld *= matScale;//���[���h�s��ɃX�P�[�����O�𔽉f
//	obj->matWorld *= matRot;
//	obj->matWorld *= matTrans;
//
//	if (obj->parent != nullptr)
//	{
//		obj->matWorld *= obj->parent->matWorld;
//	}
//
//	//�萔�o�b�t�@B0�f�[�^�]��
//	ConstBufferDataB0 *ConstMapB0 = nullptr;
//	if (SUCCEEDED(obj->constBuffB0->Map(0, nullptr, (void **)&ConstMapB0)))
//	{
//		ConstMapB0->mat = obj->matWorld * matview * NY_Object3DManager::Get()->matProjection;
//		obj->constBuffB0->Unmap(0, nullptr);
//	}
//
//	//�萔�o�b�t�@B1�f�[�^�]��
//	ConstBufferDataB1 *ConstMapB1 = nullptr;
//	if (SUCCEEDED(obj->constBuffB1->Map(0, nullptr, (void **)&ConstMapB1)))
//	{
//		ConstMapB1->amdient  = obj->model->material.ambient;
//		ConstMapB1->diffuse  = obj->model->material.diffuse;
//		ConstMapB1->specular = obj->model->material.specurar;
//		ConstMapB1->alpha    = obj->model->material.alpha;
//		obj->constBuffB1->Unmap(0, nullptr);
//	}
//
//}

void Object3d::UpdateObject3D()
{
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�s��̌v�Z

	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity();
	matWorld *= matScale;//���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot;
	matWorld *= matTrans;

	//�萔�o�b�t�@B0�f�[�^�]��
	ConstBufferDataB0 *ConstMapB0 = nullptr;
	if (SUCCEEDED(constBuffB0->Map(0, nullptr, (void **)&ConstMapB0)))
	{
		ConstMapB0->mat = matWorld * camera->GetMatrixView() * camera->GetMatrixProjection();
		ConstMapB0->color = this->color;
		constBuffB0->Unmap(0, nullptr);
	}

	//�萔�o�b�t�@B1�f�[�^�]��
	ConstBufferDataB1 *ConstMapB1 = nullptr;
	if (SUCCEEDED(constBuffB1->Map(0, nullptr, (void **)&ConstMapB1)))
	{
		ConstMapB1->amdient = model->material.ambient;
		ConstMapB1->diffuse = model->material.diffuse;
		ConstMapB1->specular = model->material.specurar;
		ConstMapB1->alpha = model->material.alpha;
		constBuffB1->Unmap(0, nullptr);
	}

	//�_�[�e�B�t���O���Z�b�g
	isDirty = false;

	model->Update();
}

//void Object3d::UpdateBillBoard3D(Object3d *obj, NY_Camera cam)
//{
//	/*
//	�r���{�[�h�̌����́A�r���[�ŉ񂷕����̔��΂�\�߂�����Ⴆ�����I����
//	NY_Camera�N���X�̌��_�Ƃ��āACamera�{�̂̃��[���h���W�͈�ؑւ��Ȃ��̂�
//	NY_Camera�̃C���X�^���X�Ńr���{�[�h�̍X�V�͏o���Ȃ��B
//	����āACamera�́u�Ǐ]����^�[�Q�b�g�v��p���ċt��]�����߂�K�v������?
//	*/
//
//
//	////�r���{�[�h�p�ϊ��s���錾
//
//
//	//���[���h�ϊ��p
//	XMMATRIX matScale, matRot, matTrans;
//
//	// �X�P�[���A��]�A���s�s��̌v�Z
//
//	matScale = XMMatrixScaling(obj->scale.x, obj->scale.y, obj->scale.z);
//
//	matRot = XMMatrixIdentity();
//	matRot *= XMMatrixRotationZ(XMConvertToRadians(obj->rotation.z));
//	matRot *= XMMatrixRotationX(XMConvertToRadians(obj->rotation.x));
//	matRot *= XMMatrixRotationY(XMConvertToRadians(obj->rotation.y));
//
//	matTrans = XMMatrixTranslation(obj->position.x, obj->position.y, obj->position.z);
//
//	obj->matWorld = XMMatrixIdentity();
//
//	obj->matWorld *= NY_Camera::Get()->GetMatrixBillBoardAll();
//
//	obj->matWorld *= matScale;
//	obj->matWorld *= matRot;
//	obj->matWorld *= matTrans;
//
//	if (obj->parent != nullptr)
//	{
//		obj->matWorld *= obj->parent->matWorld;
//	}
//
//	//�萔�o�b�t�@B0�f�[�^�]��
//	ConstBufferDataB0 *ConstMapB0 = nullptr;
//	if (SUCCEEDED(obj->constBuffB0->Map(0, nullptr, (void **)&ConstMapB0)))
//	{
//		ConstMapB0->mat = obj->matWorld * NY_Camera::Get()->GetMatrixView() * NY_Object3DManager::Get()->matProjection;
//
//		obj->constBuffB0->Unmap(0, nullptr);
//	}
//
//	//�萔�o�b�t�@B1�f�[�^�]��
//	ConstBufferDataB1 *ConstMapB1 = nullptr;
//	if (SUCCEEDED(obj->constBuffB1->Map(0, nullptr, (void **)&ConstMapB1)))
//	{
//		ConstMapB1->amdient  = obj->model->material.ambient;
//		ConstMapB1->diffuse  = obj->model->material.diffuse;
//		ConstMapB1->specular = obj->model->material.specurar;
//		ConstMapB1->alpha    = obj->model->material.alpha;
//		obj->constBuffB1->Unmap(0, nullptr);
//	}
//
//}

void Object3d::UpdateBillBoard3D()
{

	//���[���h�ϊ��p
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�s��̌v�Z

	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity();

	matWorld *= camera->GetMatrixBillBoardAll();

	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	//�萔�o�b�t�@B0�f�[�^�]��
	ConstBufferDataB0 *ConstMapB0 = nullptr;
	if (SUCCEEDED(constBuffB0->Map(0, nullptr, (void **)&ConstMapB0)))
	{
		ConstMapB0->mat = matWorld * camera->GetMatrixView() * camera->GetMatrixProjection();
		ConstMapB0->color = this->color;
		constBuffB0->Unmap(0, nullptr);
	}

	//�萔�o�b�t�@B1�f�[�^�]��
	ConstBufferDataB1 *ConstMapB1 = nullptr;
	if (SUCCEEDED(constBuffB1->Map(0, nullptr, (void **)&ConstMapB1)))
	{
		ConstMapB1->amdient		= model->material.ambient;
		ConstMapB1->diffuse		= model->material.diffuse;
		ConstMapB1->specular	= model->material.specurar;
		ConstMapB1->alpha		= model->material.alpha;
		constBuffB1->Unmap(0, nullptr);
	}

	model->Update();
}

//void Object3d::DrawModel3D(Object3d *obj, ID3D12GraphicsCommandList *cmd, ID3D12Device *dev)
//{
//	//���_�o�b�t�@�ݒ�
//	cmd->IASetVertexBuffers(0, 1, &obj->model->vbView);
//	//�C���f�b�N�X�o�b�t�@�ݒ�
//	cmd->IASetIndexBuffer(&obj->model->ibview);
//	//�萔�o�b�t�@�ݒ�
//	cmd->SetGraphicsRootConstantBufferView(0, obj->constBuffB0->GetGPUVirtualAddress());
//	//�萔�o�b�t�@�ݒ�
//	cmd->SetGraphicsRootConstantBufferView(1, obj->constBuffB1->GetGPUVirtualAddress());
//
//	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
//	cmd->SetGraphicsRootDescriptorTable(2,
//		CD3DX12_GPU_DESCRIPTOR_HANDLE(TexManager::texDsvHeap.Get()->GetGPUDescriptorHandleForHeapStart(),
//			obj->model->material.texNumber, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
//
//	//�`��
//	cmd->DrawIndexedInstanced(obj->model->indices.size(), 1, 0, 0, 0);
//}
//void Object3d::DrawModel3D(ID3D12GraphicsCommandList *cmd, ID3D12Device *dev)
//{
//	//���_�o�b�t�@�ݒ�
//	cmd->IASetVertexBuffers(0, 1, &model->vbView);
//	//�C���f�b�N�X�o�b�t�@�ݒ�
//	cmd->IASetIndexBuffer(&model->ibview);
//	//�萔�o�b�t�@�ݒ�
//	cmd->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
//	//�萔�o�b�t�@�ݒ�
//	cmd->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());
//
//	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
//	cmd->SetGraphicsRootDescriptorTable(2,
//		CD3DX12_GPU_DESCRIPTOR_HANDLE(TexManager::texDsvHeap.Get()->GetGPUDescriptorHandleForHeapStart(),
//			model->material.texNumber, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
//
//	//�`��
//	cmd->DrawIndexedInstanced(model->indices.size(), 1, 0, 0, 0);
//}


void Object3d::DrawObject()
{
	//�`��X�^���o�C
	NY_Object3DManager::Get()->SetCommonBeginDrawObject3D();
	
	//���_�o�b�t�@�ݒ�
	RAKI_DX12B_CMD->IASetVertexBuffers(0, 1, &model->vbView);
	//�C���f�b�N�X�o�b�t�@�ݒ�
	RAKI_DX12B_CMD->IASetIndexBuffer(&model->ibview);
	//�萔�o�b�t�@�ݒ�
	RAKI_DX12B_CMD->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	//�萔�o�b�t�@�ݒ�
	RAKI_DX12B_CMD->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	RAKI_DX12B_CMD->SetGraphicsRootDescriptorTable(2,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(TexManager::texDsvHeap.Get()->GetGPUDescriptorHandleForHeapStart(),
			model->material.texNumber, RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//�`��
	RAKI_DX12B_CMD->DrawIndexedInstanced(model->indices.size(), 1, 0, 0, 0);
}

void Object3d::DrawMultiPassResource()
{




}

void Object3d::LoadAndSetModelData(string modelname)
{
	//���f���f�[�^��ǂݍ���Őݒ肷��
	model.get()->LoadObjModel(modelname.c_str());
}

void Object3d::SetAnotherObjectModelData(Object3d *anotherObj)
{
	//�ʃI�u�W�F�N�g�̃��f���f�[�^���擾����
	this->model = anotherObj->model;
}

void Object3d::CreateModel_Tile(float x_size, float y_size, float x_uv, float y_uv, UINT useTexNum)
{
	model.get()->CreatePlaneModelXY(x_size, y_size, x_uv, y_uv, useTexNum, nullptr);
}

//void Object3d::DrawModel3DSelectTexture(UINT useTexNum)
//{
//	if (TexManager::textureData[useTexNum].texBuff == nullptr) {
//
//		return;
//	}
////>>>>>>> master
//
//	RAKI_DX12B_CMD->IASetVertexBuffers(0, 1, &model->vbView);
//	RAKI_DX12B_CMD->IASetIndexBuffer(&model->ibview);
//	RAKI_DX12B_CMD->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
//	RAKI_DX12B_CMD->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());
////<<<<<<< HEAD
//
//	RAKI_DX12B_CMD->SetGraphicsRootDescriptorTable(2,
//		CD3DX12_GPU_DESCRIPTOR_HANDLE(RAKI_DX12B_GET->GetMuliPassSrvDescHeap()->GetGPUDescriptorHandleForHeapStart(),
//			0, RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
//
////=======
//	RAKI_DX12B_CMD->SetGraphicsRootDescriptorTable(2,
//		CD3DX12_GPU_DESCRIPTOR_HANDLE(TexManager::texDsvHeap.Get()->GetGPUDescriptorHandleForHeapStart(),
//			useTexNum, RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
//
//	//�`��
////>>>>>>> master
//	RAKI_DX12B_CMD->DrawIndexedInstanced(model->indices.size(), 1, 0, 0, 0);
//}
