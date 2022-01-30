#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>

#include "NY_Camera.h"
#include "RVector.h"

class ParticleManager
{
private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	

public:
	// 頂点データ
	struct PVertex {
		RVector3	pos;
		float		scale;
		XMFLOAT4	color;
	};

	//定数バッファデータ構造体
	struct ConstBufferData {
		XMMATRIX mat;
		XMMATRIX matBillBoard;
	};

	// パーティクル1粒クラス
	class Particle {
		// Microsoft::WRL::を省略
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::を省略
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;

	public:
		//座標
		RVector3 pos;
		//速度
		RVector3 vel;
		//加速度
		RVector3 acc;
		//色
		XMFLOAT4 color;
		//スケーリング
		float scale;
		//回転
		float rot;
		//初期値
		XMFLOAT4 s_color = {};
		float s_scale = 1.0f;
		float s_rotation = 0.0f;
		// 最終値
		XMFLOAT4 e_color = {};
		float e_scale = 0.0f;
		float e_rotation = 0.0f;
		// 現在フレーム
		int nowFrame = 0;
		// 終了フレーム
		int endFrame = 0;
	};

private:
	// 最大生成頂点数
	static const int MAX_VERTEX = 65536;


public:
	/// <summary>
	/// パーティクルマネージャー生成
	/// </summary>
	static ParticleManager *Create();

	/// <summary>
	/// パーティクルマネージャー初期化
	/// </summary>
	void Initialize();
	
	/// <summary>
	/// 全パーティクル更新
	/// </summary>
	void Update();

	/// <summary>
	/// パーティクル描画
	/// </summary>
	void Draw(UINT drawTexNum);

	/// <summary>
	/// パーティクル追加
	/// </summary>
	void Add(int life, RVector3 pos, RVector3 vel, RVector3 acc, float startScale, float endScale, XMFLOAT4 s_color, XMFLOAT4 e_color);

private:
	//ディスクリプタヒープ、テクスチャバッファはTexManager依存

	/// メンバ変数

	// デバイス
	ID3D12Device *dev;
	// コマンド
	ID3D12GraphicsCommandList *cmd;
	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootsig;
	// グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipeline;
	// 頂点バッファ
	ComPtr <ID3D12Resource> vertbuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbview;
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	// パーティクルコンテナ
	std::forward_list<Particle> grains;

	HRESULT result = S_FALSE;


private:

	/// <summary>
	/// パーティクル用グラフィックスパイプライン初期化
	/// </summary>
	void InitializeGraphicsPipeline();

	/// <summary>
	/// パーティクル用モデル生成
	/// </summary>
	void CreateModel();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="dev">デバイス</param>
	/// <param name="cmd">コマンド</param>
	/// <param name="cam">カメラ</param>
	ParticleManager(ID3D12Device *dev, ID3D12GraphicsCommandList *cmd) {
		this->dev = dev;
		this->cmd = cmd;
	}
};

