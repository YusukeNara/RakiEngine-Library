#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <array>
#include <vector>
#include <iostream>

#include "RTex.h"
#include "Raki_imguiMgr.h"

/// <summary>
/// <para>レンダーターゲットを管理するクラス</para>
/// <para>バックバッファの管理も行うが、一番の目的はテクスチャ利用のためのレンダーターゲット作成</para>
/// <para>RenderTargetクラスの動向を管理することで、マルチパスレンダリングや</para>
/// <para>いずれ行うマルチレンダーターゲットの実装をよりわかりやすく、使いやすくするために存在</para>
/// <para>エンジンのフレームワーク化を控えてるため、シングルトンは使わない</para>
/// </summary>
class RenderTargetManager
{
private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//コンストラクタ
	RenderTargetManager();
	//デストラクタ
	~RenderTargetManager();

	//初期化
	void InitRenderTargetManager();

	//終了
	void FinalizeRenderTargetManager();

	//チェック用関数群

	//レンダーテクスチャハンドルが空かどうか？
	bool isNullHandle(int handle) { return renderTextures[handle] == nullptr; }

	//レンダーテクスチャハンドルが範囲外か？
	bool isHandleOutOfRange(int handle) { return handle < 0 || handle > renderTextures.size(); }

	/// <summary>
	/// レンダーテクスチャの生成
	/// </summary>
	/// <param name="width">テクスチャ横幅</param>
	/// <param name="height">テクスチャ縦幅</param>
	/// <returns>生成したレンダーテクスチャのハンドル</returns>
	int CreateRenderTexture(int width, int height);

	/// <summary>
	/// レンダーテクスチャをレンダーターゲットに設定
	/// </summary>
	/// <param name="handle">レンダーテクスチャのハンドル</param>
	void SetRenderTarget(int handle);

	/// <summary>
	/// レンダーテクスチャのシザー矩形を設定する
	/// <para>ここで指定したレンダーテクスチャは、ここで指定したスクリーン座標範囲外を描画しなくなる</para>
	/// <para>ただし右下座標は、予め設定した画像サイズより大きくなるような値にはならない</para>
	/// </summary>
	/// <param name="handle">設定するレンダーテクスチャハンドル</param>
	/// <param name="x1">シザー矩形左</param>
	/// <param name="y1">シザー矩形下</param>
	/// <param name="x2">シザー矩形右（画像サイズより大きい場合は自動で修正される）</param>
	/// <param name="y2">シザー矩形下（画像サイズより大きい場合は自動で修正される）</param>
	void SetRenderTargetDrawArea(int handle, int x1, int y1, int x2, int y2);

	/// <summary>
	/// レンダーテクスチャのビューポートを設定する
	/// <para>ここで指定したレンダーテクスチャは、ここで設定したビューポートの範囲内に描画される</para>
	/// <para>ビューポートについては有効活用できる場面が限られているので、そこまで気にする必要はない。</para>
	/// </summary>
	/// <param name="handle">設定するレンダーテクスチャハンドル</param>
	/// <param name="x1">ビューポート左</param>
	/// <param name="y1">ビューポート上</param>
	/// <param name="x2">ビューポート右</param>
	/// <param name="y2">ビューポート下</param>
	void SetRenderTargetClipingArea(int handle, int x1, int y1, int x2, int y2);

	/// <summary>
	/// レンダーテクスチャの共通クリアカラーを設定
	/// </summary>
	/// <param name="red"></param>
	/// <param name="green"></param>
	/// <param name="blue"></param>
	void SetClearColor(float red, float green, float blue);

	//バックバッファへの描画を開始
	void SetDrawBackBuffer();

	//スワップチェーン用バッファをフリップし、現在フレームでの描画終了コマンド実行
	void SwapChainBufferFlip();

private:
	//スワップチェーン
	ComPtr<IDXGISwapChain> swapchain;
	//バックバッファ(2つ)
	std::array<ComPtr<ID3D12Resource>, 2> backBuffers;
	//バックバッファ用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	//バックバッファ用デプス用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	//ビューポートとシザー矩形は、レンダーテクスチャを使用しない場合はデフォルトのものを使う
	CD3DX12_VIEWPORT	default_viewport;
	CD3DX12_RECT		default_rect;
	//レンダーテクスチャコンテナ
	std::vector<std::shared_ptr<RTex>> renderTextures;

	//背景クリアカラー
	float clearcolor[4] = { 0.1f,0.25f,0.5f,0.0f };

	//スワップチェーン生成関数
	void CreateSwapChain();
	//バックバッファ生成関数	
	void CreateBackBuffers();

	//現在レンダーターゲットにしているやつ
	int nowRenderTargetHandle = -1;

	//レンダーターゲットのリソースバリアを表示状態に
	void CloseDrawRenderTexture();

	//バッファフリップ
	void BufferFlip();
};
