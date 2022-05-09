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

	//�r���[�|�[�g�ƃV�U�[��`�ݒ�
	auto temp = CD3DX12_VIEWPORT(0.0f, 0.0f, Raki_WinAPI::window_width, Raki_WinAPI::window_height);
	default_viewport = temp;
	default_rect = CD3DX12_RECT(0, 0, Raki_WinAPI::window_width, Raki_WinAPI::window_height);
}

void RenderTargetManager::FinalizeRenderTargetManager()
{
	//�R���e�i����
	renderTextures.clear();
	renderTextures.shrink_to_fit();
}

int RenderTargetManager::CreateRenderTexture(int width, int height)
{
	//�V������郌���_�[�e�N�X�`��
	RTex* returnData = new RTex;

	//�e�N�X�`���f�[�^����
	returnData->CreateRTex(width, height, clearcolor);

	//�����_�[�e�N�X�`���R���e�i�Ƀf�[�^�i�[
	renderTextures.emplace_back().reset(returnData);
	//�����̃C�e���[�^�[�擾
	std::vector<std::shared_ptr<RTex>>::iterator itr = renderTextures.end();
	itr--;
	//distance���g�p���A�C���f�b�N�X�ԍ����擾
	size_t indexnum = std::distance(renderTextures.begin(), itr);

	//�L���X�g���āA�������������_�[�e�N�X�`���̃n���h����ԋp
	return static_cast<int>(indexnum);
}

void RenderTargetManager::SetRenderTarget(int handle)
{
	//�n���h���̃G���[�ɑΏ�

	//���łɃ����_�[�^�[�Q�b�g�̃n���h��
	if (handle == nowRenderTargetHandle) {
		std::cout << "WARNING : RENDERTARGETMANAGER : Pointing to an using handle. But if you know that, it is not a problem." << std::endl;
		return;
	}

	//���̒l�A�܂��͔͈͊O�Q�Ƃ͑������^�[��
	if (handle < 0 || handle > renderTextures.size()) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Render texture handle is out of range" << std::endl;
		return;
	}

	//��̃n���h���͑������^�[��
	if (renderTextures[handle] == nullptr) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Pointing to an empty handle." << std::endl;
		SetDrawBackBuffer();
		return;
	}

	//�Y���e�N�X�`���������_�[�^�[�Q�b�g�ɐݒ肷�鏈��

	//�O�̃����_�[�^�[�Q�b�g�̏I������
	CloseDrawRenderTexture();

	//�e�N�X�`���̃��\�[�X�X�e�[�g�������_�[�^�[�Q�b�g�ɕύX
	auto barrierState = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTextures[handle]->rtdata->rtexBuff.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	RAKI_DX12B_CMD->ResourceBarrier(1, &barrierState);
	
	//�f�X�N���v�^�q�[�v�ݒ�
	D3D12_CPU_DESCRIPTOR_HANDLE rtvh = renderTextures[handle]->GetDescriptorHeapRTV()->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvh = renderTextures[handle]->GetDescriptorHeapDSV()->GetCPUDescriptorHandleForHeapStart();

	//�����_�[�^�[�Q�b�g�ݒ�
	RAKI_DX12B_CMD->OMSetRenderTargets(1, &rtvh, false, &dsvh);

	//�r���[�|�[�g�A�V�U�[��`�ݒ�
	RAKI_DX12B_CMD->RSSetViewports(1, &renderTextures[handle]->viewport);
	RAKI_DX12B_CMD->RSSetScissorRects(1, &renderTextures[handle]->rect);

	//�����_�[�^�[�Q�b�g�N���A
	RAKI_DX12B_CMD->ClearRenderTargetView(rtvh, clearcolor, 0, nullptr);

	//�[�x�o�b�t�@�N���A
	RAKI_DX12B_CMD->ClearDepthStencilView(dsvh, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

void RenderTargetManager::SetRenderTargetDrawArea(int handle, int x1, int y1, int x2, int y2)
{
	//�`�F�b�N
	if (isNullHandle(handle)) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Pointing to an empty handle." << std::endl;
		return;
	}
	if (isHandleOutOfRange(handle)) {
		std::cout << "ERROR : RENDERTARGETMANAGER : Render texture handle is out of range" << std::endl;
		return;
	}

	//�摜�T�C�Y���傫���ꍇ�̓T�C�Y�ɗ}����
	

	renderTextures[handle]->rect = CD3DX12_RECT(x1, y1, x2, y2);
}

void RenderTargetManager::SetRenderTargetClipingArea(int handle, int x1, int y1, int x2, int y2)
{
	//�`�F�b�N
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
	//�����_�[�e�N�X�`���̏�Ԃ�\����Ԃ�
	CloseDrawRenderTexture();

	//�o�b�N�o�b�t�@�̕`�揀��

	//�o�b�N�o�b�t�@�̔ԍ��擾
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//�����_�[�^�[�Q�b�g�ɕύX
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	RAKI_DX12B_CMD->ResourceBarrier(1, &barrier);

	//�f�X�N���v�^�q�[�v�ݒ�
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

	//�����_�[�^�[�Q�b�g�ɐݒ�
	RAKI_DX12B_CMD->OMSetRenderTargets(1, &rtvh, false, &dsvh);
}

void RenderTargetManager::SwapChainBufferFlip()
{



}

void RenderTargetManager::CreateSwapChain()
{
	HRESULT result = S_FALSE;

	// �e��ݒ�����ăX���b�v�`�F�[���𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = Raki_WinAPI::window_width;
	swapchainDesc.Height = Raki_WinAPI::window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �F���̏�������ʓI�Ȃ��̂�
	swapchainDesc.SampleDesc.Count = 1;                 // �}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	// �o�b�N�o�b�t�@�Ƃ��Ďg����悤��
	swapchainDesc.BufferCount = 2;	                    // �o�b�t�@�����Q�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;     // �t���b�v��͑��₩�ɔj��
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // �t���X�N���[���؂�ւ�������
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

	// �e��ݒ�����ăf�B�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// �����_�[�^�[�Q�b�g�r���[
	heapDesc.NumDescriptors = swcDesc.BufferCount;
	result = RAKI_DX12B_DEV->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap));
	if (FAILED(result)) {
		assert(0);
	}

	// ���\�̂Q���ɂ���
	for (int i = 0; i < backBuffers.size(); i++)
	{
		// �X���b�v�`�F�[������o�b�t�@���擾
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (FAILED(result)) {
			assert(0);
		}

		// �f�B�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(),
			i,
			RAKI_DX12B_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		// �����_�[�^�[�Q�b�g�r���[�̐���
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
	//���݂̃����_�[�e�N�X�`����\����Ԃ�
	auto resourceBattier = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTextures[nowRenderTargetHandle]->GetTextureBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	RAKI_DX12B_CMD->ResourceBarrier(1, &resourceBattier);
}

void RenderTargetManager::BufferFlip()
{
}
