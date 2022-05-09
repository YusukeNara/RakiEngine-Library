#include "RenderTargetManager.h"
#include "Raki_WinAPI.h"
#include "Raki_DX12B.h"

RenderTargetManager::RenderTargetManager()
{

}

RenderTargetManager::~RenderTargetManager()
{
}

void RenderTargetManager::InitRenderTargetManager()
{
	CreateSwapChain();

	CreateBackBuffers();

	//ビューポートとシザー矩形設定
	auto temp = CD3DX12_VIEWPORT(0.0f, 0.0f, Raki_WinAPI::window_width, Raki_WinAPI::window_height);
	default_viewport = temp;
	default_rect = CD3DX12_RECT(0, 0, Raki_WinAPI::window_width, Raki_WinAPI::window_height);
}

void RenderTargetManager::FinalizeRenderTargetManager()
{
	//コンテナ消去
	renderTextures.clear();
	renderTextures.shrink_to_fit();
}

int RenderTargetManager::CreateRenderTexture(int width, int height)
{
	//新しく作るレンダーテクスチャ
	RTex* returnData = new RTex;

	//テクスチャデータ生成
	returnData->CreateRTex(width, height, clearcolor);

	//レンダーテクスチャコンテナにデータ格納
	renderTextures.emplace_back().reset(returnData);
	//末尾のイテレーター取得
	std::vector<std::shared_ptr<RTex>>::iterator itr = renderTextures.end();
	itr--;
	//distanceを使用し、インデックス番号を取得
	size_t indexnum = std::distance(renderTextures.begin(), itr);

	//キャストして、生成したレンダーテクスチャのハンドルを返却
	return static_cast<int>(indexnum);
}

void RenderTargetManager::SetRenderTarget(int handle)
{
	//ハンドルのエラーに対処

	//すでにレンダーターゲットのハンドル
	if (handle == nowRenderTargetHandle) {
		std::cout << "WARNING : RENDERTARGETMANAGER : Pointing to an using handle. But if you know that, it is not a problem." << std::endl;
		return;
	}

	//負の値、または範囲外参照は早期リターン
	if (handle < 0 || handle > renderTextures.size()) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Render texture handle is out of range" << std::endl;
		return;
	}

	//空のハンドルは早期リターン
	if (renderTextures[handle] == nullptr) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Pointing to an empty handle." << std::endl;
		SetDrawBackBuffer();
		return;
	}

	//該当テクスチャをレンダーターゲットに設定する処理

	//前のレンダーターゲットの終了処理
	CloseDrawRenderTexture();

	//テクスチャのリソースステートをレンダーターゲットに変更
	auto barrierState = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTextures[handle]->rtdata->rtexBuff.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	RAKI_DX12B_CMD->ResourceBarrier(1, &barrierState);
	
	//デスクリプタヒープ設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvh = renderTextures[handle]->GetDescriptorHeapRTV()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvh = renderTextures[handle]->GetDescriptorHeapDSV()->GetCPUDescriptorHandleForHeapStart();

	//レンダーターゲット設定
	RAKI_DX12B_CMD->OMSetRenderTargets(1, &rtvh, false, &dsvh);

	//ビューポート、シザー矩形設定
	RAKI_DX12B_CMD->RSSetViewports(1, &renderTextures[handle]->viewport);
	RAKI_DX12B_CMD->RSSetScissorRects(1, &renderTextures[handle]->rect);

	//レンダーターゲットクリア
	RAKI_DX12B_CMD->ClearRenderTargetView(rtvh, clearcolor, 0, nullptr);

	//深度バッファクリア
	RAKI_DX12B_CMD->ClearDepthStencilView(dsvh, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

void RenderTargetManager::SetRenderTargetDrawArea(int handle, int x1, int y1, int x2, int y2)
{
	//チェック
	if (isNullHandle(handle)) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Pointing to an empty handle." << std::endl;
		return;
	}
	if (isHandleOutOfRange(handle)) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Render texture handle is out of range" << std::endl;
		return;
	}

	//画像サイズより大きい場合はサイズに抑える
	

	renderTextures[handle]->rect = CD3DX12_RECT(x1, y1, x2, y2);
}

void RenderTargetManager::SetRenderTargetClipingArea(int handle, int x1, int y1, int x2, int y2)
{
	//チェック
	if (isNullHandle(handle)) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Pointing to an empty handle." << std::endl;
		return;
	}
	if (isHandleOutOfRange(handle)) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Render texture handle is out of range" << std::endl;
		return;
	}

	renderTextures[handle]->viewport = CD3DX12_VIEWPORT(x1, y1, x2, y2);
}

void RenderTargetManager::SetClearColor(float red, float green, float blue)
{
	clearcolor[0] = red;
	clearcolor[1] = green;
	clearcolor[2] = blue;
}

void RenderTargetManager::SetDrawBackBuffer()
{
	//レンダーテクスチャの状態を表示状態に
	CloseDrawRenderTexture();

	//バックバッファの描画準備

	//バックバッファの番号取得
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//レンダーターゲットに変更
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	RAKI_DX12B_CMD->ResourceBarrier(1, &barrier);

	//デスクリプタヒープ設定
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvh = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		bbIndex,
		RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvh = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		dsvHeap->GetCPUDescriptorHandleForHeapStart(),
		bbIndex,
		RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	);

	//レンダーターゲットに設定
	RAKI_DX12B_CMD->OMSetRenderTargets(1, &rtvh, false, &dsvh);
}

void RenderTargetManager::SwapChainBufferFlip()
{



}

void RenderTargetManager::CreateSwapChain()
{
	HRESULT result = S_FALSE;

	// 各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = Raki_WinAPI::window_width;
	swapchainDesc.Height = Raki_WinAPI::window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色情報の書式を一般的なものに
	swapchainDesc.SampleDesc.Count = 1;                 // マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	// バックバッファとして使えるように
	swapchainDesc.BufferCount = 2;	                    // バッファ数を２つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;     // フリップ後は速やかに破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // フルスクリーン切り替えを許可
	ComPtr<IDXGISwapChain1> swapchain1;
	HWND hwnd = Raki_WinAPI::GetHWND();
	result = Raki_DX12B::Get()->GetDXGIFactory()->CreateSwapChainForHwnd(
		Raki_DX12B::Get()->GetCmdQueue(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);
	if (FAILED(result)) {
		assert(0);
	}
	swapchain1.Get()->Present(0, 0);
	swapchain1.As(&swapchain);
}

void RenderTargetManager::CreateBackBuffers()
{
	HRESULT result = S_FALSE;

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
	}

	// 各種設定をしてディスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// レンダーターゲットビュー
	heapDesc.NumDescriptors = swcDesc.BufferCount;
	result = RAKI_DX12B_DEV->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap));
	if (FAILED(result)) {
		assert(0);
	}

	// 裏表の２つ分について
	for (int i = 0; i < backBuffers.size(); i++)
	{
		// スワップチェーンからバッファを取得
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (FAILED(result)) {
			assert(0);
		}

		// ディスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			i,
			RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		// レンダーターゲットビューの生成
		RAKI_DX12B_DEV->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			handle);
	}
}

void RenderTargetManager::CloseDrawBackBuffer()
{



}

void RenderTargetManager::CloseDrawRenderTexture()
{
	//現在のレンダーテクスチャを表示状態に
	auto resourceBattier = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTextures[nowRenderTargetHandle]->GetTextureBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	RAKI_DX12B_CMD->ResourceBarrier(1, &resourceBattier);
}

void RenderTargetManager::BufferFlip()
{
}
