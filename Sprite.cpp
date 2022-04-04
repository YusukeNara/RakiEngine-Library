#include "Sprite.h"
#include "NY_Camera.h"

#include "TexManager.h"

#include "Raki_DX12B.h"

void Sprite::CreateSprite(XMFLOAT2 size, XMFLOAT2 anchor, UINT resourceID, bool adjustResourceFlag, uvAnimData *animData)
{
	HRESULT result;

    if (animData != nullptr) {
        this->animData = animData;
        //頂点データ
        SpriteVertex vertices[] = {
            {{0.0f,0.0f,0.0f},this->animData->GetOffset().offsetLT},
            {{0.0f,0.0f,0.0f},this->animData->GetOffset().offsetRT},
            {{0.0f,0.0f,0.0f},this->animData->GetOffset().offsetLB},
            {{0.0f,0.0f,0.0f},this->animData->GetOffset().offsetRB},
        };

        spdata.vertices[0] = vertices[0];
        spdata.vertices[1] = vertices[1];
        spdata.vertices[2] = vertices[2];
        spdata.vertices[3] = vertices[3];
    }
    else {
        //引数がヌルならヌルを直接入れる
        this->animData = nullptr;
        //頂点データ
        SpriteVertex vertices[] = {
            {{0.0f,0.0f,0.0f},{0.0f,0.0f}},
            {{0.0f,0.0f,0.0f},{1.0f,0.0f}},
            {{0.0f,0.0f,0.0f},{0.0f,1.0f}},
            {{0.0f,0.0f,0.0f},{1.0f,1.0f}},
        };

        spdata.vertices[0] = vertices[0];
        spdata.vertices[1] = vertices[1];
        spdata.vertices[2] = vertices[2];
        spdata.vertices[3] = vertices[3];
    }

    //リソースID設定
    spdata.texNumber = resourceID;

    //アンカーポイントのコピー
    spdata.anchorPoint = anchor;



	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(SpriteVertex) * _countof(spdata.vertices));

	//頂点バッファ生成
    D3D12_HEAP_PROPERTIES heapprop{}; //ヒープ設定
    heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
    D3D12_RESOURCE_DESC resdesc{}; //リソース設定
    resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resdesc.Width = sizeVB; //頂点データ全体のサイズ
    resdesc.Height = 1;
    resdesc.DepthOrArraySize = 1;
    resdesc.MipLevels = 1;
    resdesc.SampleDesc.Count = 1;
    resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    //頂点バッファの生成
    result = SpriteManager::Get()->dev->CreateCommittedResource(
        &heapprop, //ヒープ設定
        D3D12_HEAP_FLAG_NONE,
        &resdesc, //リソース設定
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&spdata.vertBuff));

    //-----頂点バッファへのデータ転送-----//
    SpriteVertex *vertMap = nullptr;
    result = spdata.vertBuff->Map(0, nullptr, (void **)&vertMap);
    //全頂点に対して
    for (int i = 0; i < _countof(spdata.vertices); i++)
    {
        vertMap[i] = spdata.vertices[i];//座標をコピー
    }
    //マップを解除
    spdata.vertBuff->Unmap(0, nullptr);

    //頂点バッファビュー生成
    spdata.vbView.BufferLocation = spdata.vertBuff->GetGPUVirtualAddress();
    spdata.vbView.SizeInBytes = sizeof(spdata.vertices);
    spdata.vbView.StrideInBytes = sizeof(SpriteVertex);

    auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SpConstBufferData) + 0xff) & ~0xff);
    //定数バッファ生成
    result = SpriteManager::Get()->dev->CreateCommittedResource(
        &HEAP_PROP,
        D3D12_HEAP_FLAG_NONE,
        &RESDESC,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&spdata.constBuff)
    );

    //定数バッファデータ転送
    SpConstBufferData *constMap = nullptr;
    result = spdata.constBuff->Map(0, nullptr, (void **)&constMap);
    constMap->color = XMFLOAT4(1, 1, 1, 1);//色指定
    //平行投影行列
    constMap->mat = XMMatrixOrthographicOffCenterLH(0.0f, Raki_WinAPI::window_width, Raki_WinAPI::window_height, 0.0f, 0.0f, 1.0f);
    spdata.constBuff->Unmap(0, nullptr);

    //リソースに合わせて調整する場合
    if (adjustResourceFlag == true)
    {
        //テクスチャ情報取得
        D3D12_RESOURCE_DESC resDesc = TexManager::textureData[resourceID].texBuff->GetDesc();
        //リソースに合わせてサイズ調整
        spdata.size = { (float)resDesc.Width,(float)resDesc.Height };
    }
    else//しない場合
    {
        spdata.size = size;//引数のサイズに設定
    }

    ResizeSprite(spdata.size);

}

void Sprite::ResizeSprite(XMFLOAT2 newsize)
{
    HRESULT result;

    spdata.size = newsize;

    //アンカーポイントに合わせた設定
    float left   = (0.0f - spdata.anchorPoint.x) * spdata.size.x;
    float right  = (1.0f - spdata.anchorPoint.x) * spdata.size.x;
    float top    = (0.0f - spdata.anchorPoint.y) * spdata.size.y;
    float bottom = (1.0f - spdata.anchorPoint.y) * spdata.size.y;

    spdata.vertices[0].pos = { left  ,   top,0.0f };
    spdata.vertices[1].pos = { right ,   top,0.0f };
    spdata.vertices[2].pos = { left  ,bottom,0.0f };
    spdata.vertices[3].pos = { right ,bottom,0.0f };

    //頂点バッファ転送
    SpriteVertex *vertMap = nullptr;
    result = spdata.vertBuff->Map(0, nullptr, (void **)&vertMap);
    //全頂点に対して
    memcpy(vertMap, spdata.vertices, sizeof(spdata.vertices));
    //マップを解除
    spdata.vertBuff->Unmap(0, nullptr);

}

void Sprite::UpdateSprite()
{
    //アニメーション更新
    if (animData != nullptr) {
        spdata.vertices[0].uv = animData->GetOffset().offsetLB; //左上
        spdata.vertices[1].uv = animData->GetOffset().offsetRB; //左上
        spdata.vertices[2].uv = animData->GetOffset().offsetLT; //左上
        spdata.vertices[3].uv = animData->GetOffset().offsetRT; //左上
        //頂点バッファデータ転送
        SpriteVertex *vertMap = nullptr;
        auto result = spdata.vertBuff->Map(0, nullptr, (void **)&vertMap);
        //全頂点に対して
        memcpy(vertMap, spdata.vertices, sizeof(spdata.vertices));
        //マップを解除
        spdata.vertBuff->Unmap(0, nullptr);
    }

    spdata.matWorld = XMMatrixIdentity();

    spdata.matWorld *= XMMatrixRotationZ(XMConvertToRadians(spdata.rotation));

    spdata.matWorld *= XMMatrixTranslation(spdata.position.x, spdata.position.y, spdata.position.z);

    //定数バッファ転送
    SpConstBufferData *constMap = nullptr;
    HRESULT result = spdata.constBuff->Map(0, nullptr, (void **)&constMap);
    constMap->mat = spdata.matWorld * camera->GetMatrixProjection();
    constMap->color = spdata.color;
    spdata.constBuff->Unmap(0, nullptr);

}

void Sprite::Draw()
{
    //頂点バッファセット
    SpriteManager::Get()->cmd->IASetVertexBuffers(0, 1, &spdata.vbView);
    //定数バッファセット
    SpriteManager::Get()->cmd->SetGraphicsRootConstantBufferView(0, spdata.constBuff->GetGPUVirtualAddress());
    //シェーダーリソースビューをセット
    SpriteManager::Get()->cmd->SetGraphicsRootDescriptorTable(1,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(TexManager::texDsvHeap->GetGPUDescriptorHandleForHeapStart(),
        spdata.texNumber, SpriteManager::Get()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
    //描画
    SpriteManager::Get()->cmd->DrawInstanced(4, 1, 0, 0);

}

void Sprite::DrawMPRender()
{
    SpriteManager::Get()->SetCommonBeginDrawmpResource();
    //頂点バッファセット
    SpriteManager::Get()->cmd->IASetVertexBuffers(0, 1, &spdata.vbView);
    //定数バッファセット
    SpriteManager::Get()->cmd->SetGraphicsRootConstantBufferView(0, spdata.constBuff->GetGPUVirtualAddress());
    //シェーダーリソースビューをセット
    SpriteManager::Get()->cmd->SetGraphicsRootDescriptorTable(1,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(RAKI_DX12B_GET->GetMuliPassSrvDescHeap()->GetGPUDescriptorHandleForHeapStart(),
            0, RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
    //描画
    SpriteManager::Get()->cmd->DrawInstanced(4, 1, 0, 0);
}
