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
/// <para>�����_�[�^�[�Q�b�g���Ǘ�����N���X</para>
/// <para>�o�b�N�o�b�t�@�̊Ǘ����s�����A��Ԃ̖ړI�̓e�N�X�`�����p�̂��߂̃����_�[�^�[�Q�b�g�쐬</para>
/// <para>RenderTarget�N���X�̓������Ǘ����邱�ƂŁA�}���`�p�X�����_�����O��</para>
/// <para>������s���}���`�����_�[�^�[�Q�b�g�̎��������킩��₷���A�g���₷�����邽�߂ɑ���</para>
/// <para>�G���W���̃t���[�����[�N�����T���Ă邽�߁A�V���O���g���͎g��Ȃ�</para>
/// </summary>
class RenderTargetManager
{
private:
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//�R���X�g���N�^
	RenderTargetManager();
	//�f�X�g���N�^
	~RenderTargetManager();

	//������
	void InitRenderTargetManager();

	//�I��
	void FinalizeRenderTargetManager();

	//�`�F�b�N�p�֐��Q

	//�����_�[�e�N�X�`���n���h�����󂩂ǂ����H
	bool isNullHandle(int handle) { return renderTextures[handle] == nullptr; }

	//�����_�[�e�N�X�`���n���h�����͈͊O���H
	bool isHandleOutOfRange(int handle) { return handle < 0 || handle > renderTextures.size(); }

	/// <summary>
	/// �����_�[�e�N�X�`���̐���
	/// </summary>
	/// <param name="width">�e�N�X�`������</param>
	/// <param name="height">�e�N�X�`���c��</param>
	/// <returns>�������������_�[�e�N�X�`���̃n���h��</returns>
	int CreateRenderTexture(int width, int height);

	/// <summary>
	/// �����_�[�e�N�X�`���������_�[�^�[�Q�b�g�ɐݒ�
	/// </summary>
	/// <param name="handle">�����_�[�e�N�X�`���̃n���h��</param>
	void SetRenderTarget(int handle);

	/// <summary>
	/// �����_�[�e�N�X�`���̃V�U�[��`��ݒ肷��
	/// <para>�����Ŏw�肵�������_�[�e�N�X�`���́A�����Ŏw�肵���X�N���[�����W�͈͊O��`�悵�Ȃ��Ȃ�</para>
	/// <para>�������E�����W�́A�\�ߐݒ肵���摜�T�C�Y���傫���Ȃ�悤�Ȓl�ɂ͂Ȃ�Ȃ�</para>
	/// </summary>
	/// <param name="handle">�ݒ肷�郌���_�[�e�N�X�`���n���h��</param>
	/// <param name="x1">�V�U�[��`��</param>
	/// <param name="y1">�V�U�[��`��</param>
	/// <param name="x2">�V�U�[��`�E�i�摜�T�C�Y���傫���ꍇ�͎����ŏC�������j</param>
	/// <param name="y2">�V�U�[��`���i�摜�T�C�Y���傫���ꍇ�͎����ŏC�������j</param>
	void SetRenderTargetDrawArea(int handle, int x1, int y1, int x2, int y2);

	/// <summary>
	/// �����_�[�e�N�X�`���̃r���[�|�[�g��ݒ肷��
	/// <para>�����Ŏw�肵�������_�[�e�N�X�`���́A�����Őݒ肵���r���[�|�[�g�͈͓̔��ɕ`�悳���</para>
	/// <para>�r���[�|�[�g�ɂ��Ă͗L�����p�ł����ʂ������Ă���̂ŁA�����܂ŋC�ɂ���K�v�͂Ȃ��B</para>
	/// </summary>
	/// <param name="handle">�ݒ肷�郌���_�[�e�N�X�`���n���h��</param>
	/// <param name="x1">�r���[�|�[�g��</param>
	/// <param name="y1">�r���[�|�[�g��</param>
	/// <param name="x2">�r���[�|�[�g�E</param>
	/// <param name="y2">�r���[�|�[�g��</param>
	void SetRenderTargetClipingArea(int handle, int x1, int y1, int x2, int y2);

	/// <summary>
	/// �����_�[�e�N�X�`���̋��ʃN���A�J���[��ݒ�
	/// </summary>
	/// <param name="red"></param>
	/// <param name="green"></param>
	/// <param name="blue"></param>
	void SetClearColor(float red, float green, float blue);

	//�o�b�N�o�b�t�@�ւ̕`����J�n
	void SetDrawBackBuffer();

	//�X���b�v�`�F�[���p�o�b�t�@���t���b�v���A���݃t���[���ł̕`��I���R�}���h���s
	void SwapChainBufferFlip();

private:
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain> swapchain;
	//�o�b�N�o�b�t�@(2��)
	std::array<ComPtr<ID3D12Resource>, 2> backBuffers;
	//�o�b�N�o�b�t�@�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	//�o�b�N�o�b�t�@�p�f�v�X�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	//�r���[�|�[�g�ƃV�U�[��`�́A�����_�[�e�N�X�`�����g�p���Ȃ��ꍇ�̓f�t�H���g�̂��̂��g��
	CD3DX12_VIEWPORT	default_viewport;
	CD3DX12_RECT		default_rect;
	//�����_�[�e�N�X�`���R���e�i
	std::vector<std::shared_ptr<RTex>> renderTextures;

	//�w�i�N���A�J���[
	float clearcolor[4] = { 0.1f,0.25f,0.5f,0.0f };

	//�X���b�v�`�F�[�������֐�
	void CreateSwapChain();
	//�o�b�N�o�b�t�@�����֐�	
	void CreateBackBuffers();

	//���݃����_�[�^�[�Q�b�g�ɂ��Ă�����
	int nowRenderTargetHandle = -1;

	//�����_�[�^�[�Q�b�g�̃��\�[�X�o���A��\����Ԃ�
	void CloseDrawRenderTexture();

	//�o�b�t�@�t���b�v
	void BufferFlip();
};
