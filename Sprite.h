#pragma once
#include "SpriteManager.h"
#include "NY_Camera.h"
#include "TexManager.h"

class Sprite
{
public:
	//スプライト一枚の情報
	SpriteData spdata;
	//アニメーションデータ
	uvAnimData *animData;

public:
	//スプライト生成（実体生成時に起動でいい？）(デバイス、スプライトサイズ、リソース番号、アンカーポイント、スプライトマネージャーポインタ)
	Sprite(XMFLOAT2 size,UINT resourceID, bool adjustResourceFlag = false,XMFLOAT2 anchor = { 0.0f,0.0f })
	{
		//スプライト生成
		CreateSprite(size, anchor, resourceID, adjustResourceFlag, nullptr);
	};
	//引数なし（別で初期化）
	Sprite() {};

	//スプライト初期化(デバイス、スプライトサイズ、アンカーポイント、使用するリソース番号、リソース調整フラグ)
	void CreateSprite(XMFLOAT2 size, XMFLOAT2 anchor, UINT resourceID, bool adjustResourceFlag, uvAnimData *animData);

	//サイズ変更
	void ResizeSprite(XMFLOAT2 newsize);

	//スプライト更新
	void UpdateSprite();

	//スプライト描画
	void Draw();

	//マルチパス結果描画
	void DrawMPRender();

};

