#include "Sprite.h"
#include "NY_Camera.h"

#include "TexManager.h"

#include "Raki_DX12B.h"

//スプライト加工カラー
DirectX::XMFLOAT4 Sprite::sprite_color = { 1.0f,1.0f,1.0f,1.0f };

Sprite::Sprite()
{
    spdata.reset(new SpriteData);
}

Sprite::~Sprite()
{

}

void Sprite::SetSpriteColorParam(float r, float g, float b, float a)
{
    sprite_color.x = r;
    sprite_color.y = g;
    sprite_color.z = b;
    sprite_color.w = a;
}

void Sprite::CreateSprite(XMFLOAT2 size, XMFLOAT2 anchor, UINT resourceID, bool adjustResourceFlag, uvAnimData *animData)
{
	HRESULT result;

    if (animData != nullptr) {
        //this->animData = animData;
        ////���_�f�[�^
        //SpriteVertex vertices[] = {
        //    {{0.0f,0.0f,0.0f},this->animData->GetOffset().offsetLT},
        //    {{0.0f,0.0f,0.0f},this->animData->GetOffset().offsetRT},
        //    {{0.0f,0.0f,0.0f},this->animData->GetOffset().offsetLB},
        //    {{0.0f,0.0f,0.0f},this->animData->GetOffset().offsetRB},
        //};

        //spdata->vertices[0] = vertices[0];
        //spdata->vertices[1] = vertices[1];
        //spdata->vertices[2] = vertices[2];
        //spdata->vertices[3] = vertices[3];
    }

    //���\�[�XID�ݒ�
    spdata->texNumber = resourceID;

    //�A���J�[�|�C���g�̃R�s�[
    spdata->anchorPoint = anchor;

	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(SpriteVertex) * 1);

	//���_�o�b�t�@����
    D3D12_HEAP_PROPERTIES heapprop{}; //�q�[�v�ݒ�
    heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
    D3D12_RESOURCE_DESC resdesc{}; //���\�[�X�ݒ�
    resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resdesc.Width = sizeVB; //���_�f�[�^�S�̂̃T�C�Y
    resdesc.Height = 1;
    resdesc.DepthOrArraySize = 1;
    resdesc.MipLevels = 1;
    resdesc.SampleDesc.Count = 1;
    resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    //���_�o�b�t�@�̐���
    result = SpriteManager::Get()->dev->CreateCommittedResource(
        &heapprop, //�q�[�v�ݒ�
        D3D12_HEAP_FLAG_NONE,
        &resdesc, //���\�[�X�ݒ�
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&spdata->vertBuff));

    spdata->vertBuff.Get()->SetName(TEXT("SPRITE_VERTEX_BUFFER"));

    //-----���_�o�b�t�@�ւ̃f�[�^�]��-----//
    SpriteVertex *vertMap = nullptr;
    result = spdata->vertBuff->Map(0, nullptr, (void **)&vertMap);
    //�S���_�ɑ΂���
    vertMap = &spdata->vertice;//���W���R�s�[
    //�}�b�v������
    spdata->vertBuff->Unmap(0, nullptr);

    //���_�o�b�t�@�r���[����
    spdata->vbView.BufferLocation = spdata->vertBuff->GetGPUVirtualAddress();
    spdata->vbView.SizeInBytes = sizeof(spdata->vertice);
    spdata->vbView.StrideInBytes = sizeof(SpriteVertex);

    //�C���X�^���V���O�p���_�o�b�t�@����

    //�������p
    SpriteInstance spins[] = {
        {XMMatrixIdentity()},
    };

    sizeInsVB = static_cast<UINT>(sizeof(SpriteInstance) * 32);

    auto INS_HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC INS_RESDESC{};
    INS_RESDESC.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    INS_RESDESC.Width = sizeInsVB; //���_�f�[�^�S�̂̃T�C�Y
    INS_RESDESC.Height = 1;
    INS_RESDESC.DepthOrArraySize = 1;
    INS_RESDESC.MipLevels = 1;
    INS_RESDESC.SampleDesc.Count = 1;
    INS_RESDESC.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    //���_�o�b�t�@�̐���
    result = SpriteManager::Get()->dev->CreateCommittedResource(
        &INS_HEAP_PROP, //�q�[�v�ݒ�
        D3D12_HEAP_FLAG_NONE,
        &INS_RESDESC, //���\�[�X�ݒ�
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&spdata->vertInsBuff));

    spdata->vertInsBuff.Get()->SetName(TEXT("SPRITE_VERTEX_INSTANCING_DATA"));

    //�f�[�^�]��
    SpriteInstance *insmap = nullptr;
    result = spdata->vertInsBuff.Get()->Map(0, nullptr, (void **)&insmap);
    for (int i = 0; i < _countof(spins); i++) {
        insmap[i].worldmat = spins[i].worldmat * camera->GetMatrixViewProjection();
    }
    spdata->vertInsBuff.Get()->Unmap(0, nullptr);

    //�r���[�쐬
    spdata->vibView.BufferLocation = spdata->vertInsBuff->GetGPUVirtualAddress();
    spdata->vibView.SizeInBytes = sizeof(spins);
    spdata->vibView.StrideInBytes = sizeof(SpriteInstance);


    auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SpConstBufferData) + 0xff) & ~0xff);
    //�萔�o�b�t�@����
    result = SpriteManager::Get()->dev->CreateCommittedResource(
        &HEAP_PROP,
        D3D12_HEAP_FLAG_NONE,
        &RESDESC,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&spdata->constBuff)
    );

    //�萔�o�b�t�@�f�[�^�]��
    SpConstBufferData *constMap = nullptr;
    result = spdata->constBuff->Map(0, nullptr, (void **)&constMap);
    constMap->color = XMFLOAT4(1, 1, 1, 1);//�F�w��
    //���s���e�s��
    constMap->mat = XMMatrixOrthographicOffCenterLH(0.0f, Raki_WinAPI::window_width, Raki_WinAPI::window_height, 0.0f, 0.0f, 1.0f);
    spdata->constBuff->Unmap(0, nullptr);

    //���\�[�X�ɍ��킹�Ē�������ꍇ
    if (adjustResourceFlag == true)
    {
        //�e�N�X�`�����擾
        D3D12_RESOURCE_DESC resDesc = TexManager::textureData[resourceID].texBuff->GetDesc();
        //���\�[�X�ɍ��킹�ăT�C�Y����
        spdata->size = { (float)resDesc.Width,(float)resDesc.Height };
    }
    else//���Ȃ��ꍇ
    {
        spdata->size = size;//�����̃T�C�Y�ɐݒ�
    }

}

void Sprite::Create(UINT resourceID)
{

    HRESULT result;

    //���_�f�[�^
    SpriteVertex vertices = {
        {0.0f,0.0f,0.0f},{0.0f,0.0f},
    };

    spdata->vertice = vertices;

    //�e�N�X�`���ݒ�
    spdata->texNumber = resourceID;

    //���_�f�[�^�ƃC���f�b�N�X�f�[�^�𐶐����čX�V

    //���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
    UINT sizeVB = static_cast<UINT>(sizeof(SpriteVertex) * 2);
    //���_�o�b�t�@����
    D3D12_HEAP_PROPERTIES heapprop{}; //�q�[�v�ݒ�
    heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
    D3D12_RESOURCE_DESC resdesc{}; //���\�[�X�ݒ�
    resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resdesc.Width = sizeVB; //���_�f�[�^�S�̂̃T�C�Y
    resdesc.Height = 1;
    resdesc.DepthOrArraySize = 1;
    resdesc.MipLevels = 1;
    resdesc.SampleDesc.Count = 1;
    resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    //���_�o�b�t�@�̐���
    result = SpriteManager::Get()->dev->CreateCommittedResource(
        &heapprop, //�q�[�v�ݒ�
        D3D12_HEAP_FLAG_NONE,
        &resdesc, //���\�[�X�ݒ�
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&spdata->vertBuff));

    spdata->vertBuff.Get()->SetName(TEXT("SPRITE_VERTEX_DATA"));

    //-----���_�o�b�t�@�ւ̃f�[�^�]��-----//
    SpriteVertex *vertMap = nullptr;
    result = spdata->vertBuff->Map(0, nullptr, (void **)&vertMap);
    //�S���_�ɑ΂���
    vertMap = &spdata->vertice;//���W���R�s�[
    //�}�b�v������
    spdata->vertBuff->Unmap(0, nullptr);
    //���_�o�b�t�@�r���[����
    spdata->vbView.BufferLocation = spdata->vertBuff->GetGPUVirtualAddress();
    spdata->vbView.SizeInBytes = sizeof(spdata->vertice);
    spdata->vbView.StrideInBytes = sizeof(SpriteVertex);

    //�C���X�^���V���O�p���_�o�b�t�@����
    //�������p
    SpriteInstance spins[] = {
        {XMMatrixIdentity()},
    };
    sizeInsVB = static_cast<UINT>(sizeof(SpriteInstance) * 8);
    auto INS_HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC INS_RESDESC{};
    INS_RESDESC.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    INS_RESDESC.Width = sizeInsVB; //���_�f�[�^�S�̂̃T�C�Y
    INS_RESDESC.Height = 1;
    INS_RESDESC.DepthOrArraySize = 1;
    INS_RESDESC.MipLevels = 1;
    INS_RESDESC.SampleDesc.Count = 1;
    INS_RESDESC.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    //���_�o�b�t�@�̐���
    result = SpriteManager::Get()->dev->CreateCommittedResource(
        &INS_HEAP_PROP, //�q�[�v�ݒ�
        D3D12_HEAP_FLAG_NONE,
        &INS_RESDESC, //���\�[�X�ݒ�
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&spdata->vertInsBuff));

    spdata->vertInsBuff.Get()->SetName(TEXT("SPRITE_VERTEX_INSTANCING_DATA"));

    //�f�[�^�]��
    SpriteInstance *insmap = nullptr;
    result = spdata->vertInsBuff->Map(0, nullptr, (void **)&insmap);
    for (int i = 0; i < _countof(spins); i++) {
        insmap[i].worldmat = spins[i].worldmat * camera->GetMatrixProjection();
    }
    spdata->vertInsBuff->Unmap(0, nullptr);

    //�r���[�쐬
    spdata->vibView.BufferLocation = spdata->vertInsBuff->GetGPUVirtualAddress();
    spdata->vibView.SizeInBytes = sizeof(spins);
    spdata->vibView.StrideInBytes = sizeof(SpriteInstance);
    auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SpConstBufferData) + 0xff) & ~0xff);

    //�萔�o�b�t�@����
    result = SpriteManager::Get()->dev->CreateCommittedResource(
        &HEAP_PROP,
        D3D12_HEAP_FLAG_NONE,
        &RESDESC,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&spdata->constBuff)
    );

    spdata->constBuff.Get()->SetName(TEXT("SPRITE_CONST_BUFFER"));

    //�萔�o�b�t�@�f�[�^�]��
    SpConstBufferData *constMap = nullptr;
    result = spdata->constBuff->Map(0, nullptr, (void **)&constMap);
    constMap->color = XMFLOAT4(1, 1, 1, 1);//�F�w��

    //���s���e�s��
    constMap->mat = XMMatrixOrthographicOffCenterLH(0.0f, Raki_WinAPI::window_width, Raki_WinAPI::window_height, 0.0f, 0.0f, 1.0f);
    spdata->constBuff->Unmap(0, nullptr);

    //�e�N�X�`���̃f�t�H���g�T�C�Y���擾
    TEXTURE_DEFAULT_SIZE.x = TexManager::textureData[resourceID].metaData.width;
    TEXTURE_DEFAULT_SIZE.y = TexManager::textureData[resourceID].metaData.height;

    //デフォルトのuvを格納
    spdata->uvOffsets.push_back(XMFLOAT4(0.0, 0.0, 1.0, 1.0));

    //スプライト生成炭
    isCreated = true;
}

void Sprite::CreateAndSetDivisionUVOffsets(int divAllnum, int divX, int divY, int sizeX, int sizeY, UINT resourceID)
{
    //負の値は無効
    if (divAllnum < 0 || divX < 0 || divY < 0 || sizeX < 0 || sizeY < 0) {
        std::cout << "ERROR : SPRITE : CreateAndSetDivisionUVOffsets() : Invalid value." << std::endl;
        return;
    }

    //uv分割が意図しない値にならないかチェック


    //スプライトデータ作成
    Create(resourceID);
    //デフォルトサイズを変更
    TEXTURE_DEFAULT_SIZE.x = sizeX;
    TEXTURE_DEFAULT_SIZE.y = sizeY;

    //初期化したコンテナを一旦クリア
    spdata->uvOffsets.clear();
    spdata->uvOffsets.shrink_to_fit();

    //uvoffsetを計算
    float x_uvOffset = static_cast<float>(sizeX) / TexManager::textureData[resourceID].metaData.width;
    float y_uvOffset = static_cast<float>(sizeY) / TexManager::textureData[resourceID].metaData.height;

    int dived = 0;
    //インスタンシング用オフセットコンテナに格納
    for (int y = 0; y < divY; y++) {
        for (int x = 0; x < divX; x++) {
            float left = x * x_uvOffset;
            float right = x * x_uvOffset + x_uvOffset;
            float top = y * y_uvOffset;
            float bottom = y * y_uvOffset + y_uvOffset;
            spdata->uvOffsets.push_back(XMFLOAT4(left, top, right, bottom));
            dived++;
            if (dived > divAllnum) { break; }
        }
    }

}

void Sprite::UpdateSprite()
{
    //�A�j���[�V�����X�V
    //if (animData != nullptr) {
    //    spdata->vertices[0].uv = animData->GetOffset().offsetLB; //����
    //    spdata->vertices[1].uv = animData->GetOffset().offsetRB; //����
    //    spdata->vertices[2].uv = animData->GetOffset().offsetLT; //����
    //    spdata->vertices[3].uv = animData->GetOffset().offsetRT; //����
    //    //���_�o�b�t�@�f�[�^�]��
    //    SpriteVertex *vertMap = nullptr;
    //    auto result = spdata->vertBuff->Map(0, nullptr, (void **)&vertMap);
    //    //�S���_�ɑ΂���
    //    memcpy(vertMap, spdata->vertices, sizeof(spdata->vertices));
    //    //�}�b�v������
    //    spdata->vertBuff->Unmap(0, nullptr);
    //}

    spdata->matWorld = XMMatrixIdentity();

    spdata->matWorld *= XMMatrixRotationZ(XMConvertToRadians(spdata->rotation));

    spdata->matWorld *= XMMatrixTranslation(spdata->position.x, spdata->position.y, spdata->position.z);

    //�萔�o�b�t�@�]��


}

void Sprite::InstanceUpdate()
{
    //�`�搔�ɍ��킹�ĉς�����
    spdata->vibView.SizeInBytes = spdata->insWorldMatrixes.size() * sizeof(SpriteInstance);
    
    //�C���X�^���V���O���_�o�b�t�@�̃T�C�Y��ύX����K�v������ꍇ
    if (isVertexBufferNeedResize()) {

        ResizeVertexInstanceBuffer(spdata->insWorldMatrixes.size() * sizeof(SpriteInstance));
    }

    //�o�b�t�@�f�[�^�]��
    SpriteInstance *insmap = nullptr;
    auto result = spdata->vertInsBuff->Map(0, nullptr, (void **)&insmap);
    for (int i = 0; i < spdata->insWorldMatrixes.size(); i++) {
        insmap[i].worldmat = spdata->insWorldMatrixes[i].worldmat * camera->GetMatrixProjection2D();
        insmap[i].drawsize = spdata->insWorldMatrixes[i].drawsize;
        insmap[i].uvOffset = spdata->uvOffsets[uvOffsetHandle];
        insmap[i].color = spdata->insWorldMatrixes[i].color;
    }
    spdata->vertInsBuff->Unmap(0, nullptr);

    SpConstBufferData* constMap = nullptr;
    result = spdata->constBuff->Map(0, nullptr, (void**)&constMap);
    constMap->mat = spdata->matWorld * camera->GetMatrixProjection();
    constMap->color = spdata->color;
    spdata->constBuff->Unmap(0, nullptr);
}

void Sprite::Draw()
{
    SpriteManager::Get()->SetCommonBeginDraw();

    //�C���X�^���V���O�f�[�^�X�V
    InstanceUpdate();

    //���_�o�b�t�@�Z�b�g
    D3D12_VERTEX_BUFFER_VIEW vbviews[] = {
        spdata->vbView,spdata->vibView
    };
    SpriteManager::Get()->cmd->IASetVertexBuffers(0, _countof(vbviews), vbviews);
    //�萔�o�b�t�@�Z�b�g
    SpriteManager::Get()->cmd->SetGraphicsRootConstantBufferView(0, spdata->constBuff->GetGPUVirtualAddress());
    //�V�F�[�_�[���\�[�X�r���[���Z�b�g
    SpriteManager::Get()->cmd->SetGraphicsRootDescriptorTable(1,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(TexManager::texDsvHeap->GetGPUDescriptorHandleForHeapStart(),
        spdata->texNumber, SpriteManager::Get()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
    //�`��
    SpriteManager::Get()->cmd->DrawInstanced(1, (UINT)spdata->insWorldMatrixes.size(), 0, 0);

    //�C���X�^���X�f�[�^���N���A���A�R���e�i���Z�b�g
    spdata->insWorldMatrixes.clear();
    spdata->insWorldMatrixes.shrink_to_fit();
}

void Sprite::DrawSprite(float posX, float posY)
{
    //���W�����Ƃɕ��s�ړ��s��쐬
    XMMATRIX trans = XMMatrixTranslation(posX, posY, 0);
    //��]�A�X�P�[�����O�͂Ȃ�
    XMMATRIX norot = XMMatrixRotationZ(XMConvertToRadians(0.0f));

    //�s��R���e�i�Ɋi�[
    SpriteInstance ins = {};
    ins.worldmat = XMMatrixIdentity();
    ins.worldmat *= norot;
    ins.worldmat *= trans;
    //�f�t�H���g�T�C�Y���i�[
    ins.drawsize = TEXTURE_DEFAULT_SIZE;
    ins.color = sprite_color;
    spdata->insWorldMatrixes.push_back(ins);
}

void Sprite::DrawExtendSprite(float x1, float y1, float x2, float y2)
{
    //���W�����Ƃɕ��s�ړ��s����쐬
    XMMATRIX trans = XMMatrixTranslation(x1, y1, 0);
    //��]�A�X�P�[�����O�͂Ȃ�
    XMMATRIX norot = XMMatrixRotationZ(XMConvertToRadians(0.0f));
    XMMATRIX noScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

    //�s��R���e�i�Ɋi�[
    SpriteInstance ins = {};

    ins.worldmat = XMMatrixIdentity();
    ins.worldmat *= norot;
    ins.worldmat *= trans;
    ins.drawsize = { x2 - x1, y2 - y1 };
    //�s��R���e�i�Ɋi�[
    ins.color = sprite_color;
    spdata->insWorldMatrixes.push_back(ins);
}

void Sprite::DrawMPRender()
{
    SpriteManager::Get()->SetCommonBeginDrawmpResource();
    //���_�o�b�t�@�Z�b�g
    SpriteManager::Get()->cmd->IASetVertexBuffers(0, 1, &spdata->vbView);
    //�萔�o�b�t�@�Z�b�g
    SpriteManager::Get()->cmd->SetGraphicsRootConstantBufferView(0, spdata->constBuff->GetGPUVirtualAddress());
    //�V�F�[�_�[���\�[�X�r���[���Z�b�g
    SpriteManager::Get()->cmd->SetGraphicsRootDescriptorTable(1,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(RAKI_DX12B_GET->GetMuliPassSrvDescHeap()->GetGPUDescriptorHandleForHeapStart(),
            0, RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
    //�`��
    SpriteManager::Get()->cmd->DrawInstanced(4, 1, 0, 0);
}

bool Sprite::IsCreated()
{
    return isCreated;
}

bool Sprite::isVertexBufferNeedResize()
{
    return spdata->vertInsBuff.Get()->GetDesc().Width < spdata->insWorldMatrixes.size() * sizeof(SpriteInstance);
}

void Sprite::ResizeVertexInstanceBuffer(UINT newWidthSize)
{
    //���_�o�b�t�@�̐ݒ�
    auto INS_HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC INS_RESDESC{};
    INS_RESDESC.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    INS_RESDESC.Width = newWidthSize; //���_�f�[�^�S�̂̃T�C�Y
    INS_RESDESC.Height = 1;
    INS_RESDESC.DepthOrArraySize = 1;
    INS_RESDESC.MipLevels = 1;
    INS_RESDESC.SampleDesc.Count = 1;
    INS_RESDESC.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    //���_�o�b�t�@�̍Đ���
    auto result = SpriteManager::Get()->dev->CreateCommittedResource(
        &INS_HEAP_PROP, //�q�[�v�ݒ�
        D3D12_HEAP_FLAG_NONE,
        &INS_RESDESC, //���\�[�X�ݒ�
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&spdata->vertInsBuff));

    spdata->vertInsBuff.Get()->SetName(TEXT("SPRITE_VERTEX_INSTANCE_RESIZED"));

    spdata->vibView.BufferLocation = spdata->vertInsBuff.Get()->GetGPUVirtualAddress();
    spdata->vibView.SizeInBytes = newWidthSize;

}
