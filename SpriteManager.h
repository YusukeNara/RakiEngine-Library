#pragma once
#include <iostream>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <d3dcompiler.h>
#include <wrl.h>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

//スプライト用モデル頂点構造体
typedef struct SpriteVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
}SPVertex;

//スプライト用インスタンス構造体描画するものそれぞれの差分データ
typedef struct SpriteInstance
{
	XMMATRIX worldmat;	//ワールド変換行列
	XMFLOAT2 drawsize;	//縦横幅
};

//定数バッファデータ構造体
typedef struct SpConstBufferData
{
	XMFLOAT4 color;//色(rgba)
	XMMATRIX mat;
}SPConstBuffer;

//スプライト一枚分のデータ
typedef struct SpriteData
{
	//頂点座標（一点のみにして、ジオメトリで細かいコントロールをする）
	SpriteVertex vertice;
	//インスタンス行列コンテナ（差分データを格納：アフィン変換もあるし、ここに縦横幅のデータ入れてもいいかも）
	std::vector<XMMATRIX> insWorldMatrixes;

	ComPtr<ID3D12Resource> vertBuff;//モデル用頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView;//モデル用頂点バッファビュー

	ComPtr<ID3D12Resource> vertInsBuff;	//インスタンス用頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vibView;	//インスタンス用頂点バッファビュー

	ComPtr<ID3D12Resource> constBuff;//定数バッファ
	UINT texNumber;//マネージャーに保存されたリソースの番号

	XMFLOAT2 size;//スプライトサイズ

	float rotation = 0.0f;//z軸回転角
	XMFLOAT3 position = { 0.0f,0.0f,0.0f };//座標
	XMMATRIX matWorld;//ワールド変換行列

	XMFLOAT2 anchorPoint = { 0.0f,0.0f };//アンカーポイント

	XMFLOAT2 screenPos = { 0.0f,0.0f };//スクリーン座標

	XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

}SPData;

//スプライト共通管理クラス
//Winmainでのspritemanagerの実体は1つのみ作ること！

//仕様
/*
	DrawSprite()の引数にSpriteクラスのインスタンスを取り込むことで描画
*/

//スプライト用テクスチャの最大読み込み数
const int MAX_TEX_NUM = 512;

class SpriteManager
{
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


private:
	//スプライト用グラフィックスパイプラインセット
	ComPtr<ID3D12PipelineState> pipelinestate;
	ComPtr<ID3D12RootSignature> rootsignature;

	//マルチパスエフェクト用グラフィクスパイプラインセット
	ComPtr<ID3D12PipelineState> mpPipeline;
	ComPtr<ID3D12RootSignature> mpRootsig;

	SpriteManager(int window_width, int window_height) {
		//ビューポート行列初期化
		matViewport.r[0].m128_f32[0] = window_width / 2;
		matViewport.r[1].m128_f32[1] = -window_height / 2;
		matViewport.r[3].m128_f32[0] = window_width / 2;
		matViewport.r[3].m128_f32[1] = window_height / 2;
		//パイプライン生成
		CreateSpritePipeline();
	};
	SpriteManager() {};
	~SpriteManager() {};

public: 
	//共通ビューポート行列
	XMMATRIX matViewport{};


	void CreateSpriteManager(ID3D12Device *dev, ID3D12GraphicsCommandList *cmd, int window_w, int window_h);
	//スプライトのグラフィックスパイプラインを生成
	void CreateSpritePipeline();
	//スプライト共通のグラフィックスコマンドをセット
	void SetCommonBeginDraw();
	//マルチパスリソース描画用グラフィクスコマンド
	void SetCommonBeginDrawmpResource();

	//インスタンス取得
	static SpriteManager *Get() {
		static SpriteManager mgr;
		return &mgr;
	}
	//描画に必要なポインタ
	ID3D12Device *dev;
	ID3D12GraphicsCommandList *cmd;

	//コピーコンストラクタ、代入演算子無効化
	SpriteManager &operator=(const SpriteManager &obj) = delete;
	SpriteManager(const SpriteManager &obj) = delete;

};

