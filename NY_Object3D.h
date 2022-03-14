#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "NY_Object3DMgr.h"
#include "RVector.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

//定数バッファデータ構造体B0
struct ConstBufferDataB0
{
	XMMATRIX mat;
	XMFLOAT4 color;
};

//定数バッファデータ構造体B1
struct ConstBufferDataB1
{
	XMFLOAT3 amdient;  //アンビエント
	float pad1;//パディング
	XMFLOAT3 diffuse;  //ディフューズ
	float pad2;//パディング
	XMFLOAT3 specular; //スペキュラー
	float alpha;       //アルファ
};

//ビルボードパーティクル頂点データ
struct ParticleVertex
{
	XMFLOAT3 pos;
};

extern class Model3D;
extern class NY_Camera;
extern class NY_Object3DManager;


class Object3d
{
public:
	//オブジェクトの名前
	string tagName;

	//適用するリソースの番号
	UINT resourceNumber;

	//モデルデータ
	Model3D *model;

	//定数バッファ
	ComPtr<ID3D12Resource> constBuffB0;
	ComPtr<ID3D12Resource> constBuffB1;

	//定数バッファビューハンドル(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;

	//定数バッファビューハンドル(GPU)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;

	//アフィン変換情報
	RVector3 scale    = { 1,1,1 };
	RVector3 rotation = { 0,0,0 };
	RVector3 position = { 0,0,0 };

	//ワールド変換行列
	XMMATRIX matWorld;

	//親子(親要素へのポインタ)
	Object3d *parent = nullptr;

	//色情報
	XMFLOAT4 color = { 1,1,1,1 };

	//ビルボードフラグ
	bool isBillBoard;

	//ダーティフラグ
	bool isDirty;

public:
	//コンストラクタ
	Object3d() {
		isBillBoard = false;
		isDirty = true;
	};

	//オブジェクトの初期化
	//static void InitObject3D(Object3d *obj, ID3D12Device *dev);
	void InitObject3D(ID3D12Device *dev);

	//ロード済モデルデータの設定
	//static void SetLoadedModelData(Object3d *obj, NY_Model3D *loadedModel);
	void SetLoadedModelData(Model3D *loadedModel);

	//オブジェクト更新
	//static void UpdateObject3D(Object3d *obj, XMMATRIX &matview);
	void UpdateObject3D();

	//ビルボード更新（カメラオブジェクトをそのまま取り込んで、ビルボード用の更新処理を行う）
	//static void UpdateBillBoard3D(Object3d *obj, NY_Camera cam);
	void UpdateBillBoard3D();

	//モデルデータを使用したオブジェクト描画
	static void DrawModel3D(Object3d *obj, ID3D12GraphicsCommandList *cmd,ID3D12Device *dev);
	void DrawModel3D(ID3D12GraphicsCommandList *cmd, ID3D12Device *dev);
	void DrawModel3DSelectTexture(UINT useTexNum);

	//マルチパスレンダリングを使用した描画（マルチパスで作ったリソースを使って描画）
	void DrawMultiPassResource();

private:
	/// プライベートメンバ変数

};





