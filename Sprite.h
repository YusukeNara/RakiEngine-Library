#pragma once
#include "SpriteManager.h"
#include "NY_Camera.h"


class Sprite
{
public:
	//スプライト一枚の情報
	SpriteData spdata;



public:
	//スプライト生成（実体生成時に起動でいい？）(デバイス、スプライトサイズ、リソース番号、アンカーポイント、スプライトマネージャーポインタ)
	Sprite(ID3D12Device *dev,XMFLOAT2 size,UINT resourceID, bool adjustResourceFlag = false,XMFLOAT2 anchor = { 0.0f,0.0f },SpriteManager *mgr = nullptr)
	{
		//スプライト生成
		CreateSprite(dev, size, anchor, resourceID, adjustResourceFlag);
	};
	//引数なし（別で初期化）
	Sprite() {};

	//スプライトマネージャー設定(スプライトマネージャーの初期化を終わらせてから呼び出すこと)
	static void SetSpriteManager(SpriteManager *mgr)
	{

	}

	//スプライト初期化(デバイス、スプライトサイズ、アンカーポイント、使用するリソース番号、リソース調整フラグ)
	void CreateSprite(ID3D12Device *dev, XMFLOAT2 size, XMFLOAT2 anchor,UINT resourceID,bool adjustResourceFlag);

	//サイズ変更
	void ResizeSprite(XMFLOAT2 newsize);

	//スプライト更新
	void UpdateSprite();

	//スプライト描画
	void Draw(ID3D12GraphicsCommandList *cmd, ID3D12Device *dev);

};

