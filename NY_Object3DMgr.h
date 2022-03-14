#pragma once
#include <iostream>
#include <vector>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXTex.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <vector>
#include <memory>

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")


#include "NY_Camera.h"
#include "NY_Object3D.h"
#include "NY_Model.h"
#include "RVector.h"


typedef struct Pipeline3D
{
	//パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState> pipelinestate;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootsignature;

}PipelineSet;

/// 12/14
///データの管理をマネージャーが一括で行うようにするために
///NY_Object3dを構造体化し、マネージャーがコンテナで管理できるようにする
/// 
/// テクスチャデータをtexManagerで管理するように変更


extern class Object3d;
extern class NY_Camera;

/// <summary>
/// 一つだけ使えればいいのでシングルトンパターン
/// 
/// </summary>
class NY_Object3DManager
{
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//----------パイプラインステートオブジェクト----------//

	//標準パイプラインステート
	Pipeline3D object3dPipelineSet;
	//即時描画用パイプライン
	Pipeline3D quickDrawPipelineSet;


	//----------パイプラインステート構造体--------------//

	//パイプラインステート構造体を複数作って、適宜割り当てる形でもいいのか？

	//グラフィックスパイプラインステート構造体
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipelineStateDesc;


	//----------シェーダーオブジェクト----------//

	//シェーダーオブジェクト（標準）
	ComPtr<ID3DBlob> obj_VS;
	ComPtr<ID3DBlob> obj_GS;
	ComPtr<ID3DBlob> obj_PS;

	//シェーダーオブジェクト（即時描画用）
	ComPtr<ID3DBlob> qd_BoxGS;


	//----------object3dコンテナ----------//
	vector<Object3d*> objects;


	//----------NY_Cameraクラスのポインタ----------//


	//----------その他----------//

	//デフォルトテクスチャの色
	XMFLOAT4 defColor = { 1.0,0.0,0.0,1.0 };

	//コンストラクタ、デストラクタ
	NY_Object3DManager() {};
	~NY_Object3DManager() {};

public:
	//コピーコンストラクタ無効化
	NY_Object3DManager(const NY_Object3DManager &obj) = delete;
	//代入演算子無効化
	NY_Object3DManager &operator=(const NY_Object3DManager &obj) = delete;

	//インスタンス取得
	static NY_Object3DManager *Get() {
		static NY_Object3DManager obj;
		return &obj;
	}

	//共通射影行列
	XMMATRIX matProjection{};
	//共通ビューポート行列
	XMMATRIX matViewport{};
	//テクスチャ用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descheap;
	//テクスチャリソース配列
	ComPtr<ID3D12Resource> texbuff[256];
	//ウィンドウサイズ
	int window_width, window_height;

	/// <summary>
	/// <para>NY_Object3DManagerの生成</para>
	/// </summary>
	/// <param name="dev">ID3D12Deviceのポインタ</param>
	/// <param name="window_width">ウィンドウ横幅</param>
	/// <param name="window_height">ウィンドウ縦幅</param>
	/// <returns>成否</returns>
	bool CreateObject3DManager(ID3D12Device *dev, int window_w, int window_h);

	/// <summary>
	/// 貼り付けるテクスチャの読み込み
	/// </summary>
	/// <param name="texNumber">テクスチャ番号を取得する変数</param>
	/// <param name="filename">ロードするテクスチャのパス（）</param>
	/// <param name="dev"></param>
	void LoadObject3DTexture(UINT &texNumber, string filename, ID3D12Device *dev);

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="cam">NY_Camera変数</param>

	/// <summary>
	/// object3dの作成、コンテナに格納
	/// </summary>
	Object3d *CreateObject3d(Model3D *modelData);

	/// <summary>
	/// オブジェクトの削除
	/// </summary>
	/// <param name="obj">消すObject3Dのポインタ</param>
	void DeleteObject3d(Object3d *obj);

	/// <summary>
	/// 生成済オブジェクトの一括更新
	/// </summary>
	void UpdateAllObjects();

	/// <summary>
	/// 3Dオブジェクトの描画を実行するための前準備を行う
	/// </summary>
	/// <param name="cmd">ID3D12GraphicsCommandListのポインタ</param>
	void SetCommonBeginDrawObject3D();

	void SetCommonBeginDrawObject3D2MultiPassRenderResource();

	static ID3D12Device *GetDev() {
		return dev;
	}

private:

	//そのフレームで更新を行ったか？
	bool isUpdated = false;

	//デバイスのポインタ
	static ID3D12Device *dev;


	/// <summary>
	/// 即時描画用ジオメトリシェーダーのコンパイル（生成関数で実行）
	/// </summary>
	/// <returns>全シェーダーコンパイルの成否</returns>
	HRESULT QuickDrawShadersCompile();

	/// <summary>
	/// クラス内で使用するグラフィックスパイプラインの生成
	/// </summary>
	/// <param name="dev">ID3D12Deviceのポインタ</param>
	/// <returns>作成したPipeline3D構造体</returns>
	Pipeline3D Create3DPipelineState(ID3D12Device *dev);


};

/// ----------- 記述量の削減のためのラップ関数群 ---------- ///

/// <summary>
/// NY_Object3DManagerの初期化
/// </summary>
/// <param name="dev">ID3D12Deviceのポインタ</param>
/// <param name="window_w">ウィンドウ横幅</param>
/// <param name="window_h">ウィンドウ縦幅</param>
inline void InitializeObject3DManager(ID3D12Device *dev, int window_w, int window_h) {

	//NY_Object3DManagerを生成、初期化
	NY_Object3DManager::Get()->CreateObject3DManager(dev, window_w, window_h);
}

/// <summary>
/// Object3dの生成、管理クラスのコンテナに格納
/// </summary>
/// <param name="modelData">NY_Model3Dのモデルデータ</param>
/// <param name="pos">初期位置を設定する</param>
/// <returns>生成されたObject3dを示すポインタ</returns>
Object3d *CreateObject3d(Model3D *modelData, RVector3 pos = { 0,0,0 });

/// <summary>
/// Object3dデータの削除
/// </summary>
/// <param name="obj"></param>
inline void DeleteObject3d(Object3d *obj) {

	NY_Object3DManager::Get()->DeleteObject3d(obj);
}

inline void UpdateObject() {
	//全オブジェクトの更新
	NY_Object3DManager::Get()->UpdateAllObjects();
}

//描画関数
void DrawObject3d(Object3d *obj);