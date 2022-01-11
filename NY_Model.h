#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

#include "NY_Object3DMgr.h"

//頂点データ
struct Vertex
{
	XMFLOAT3 pos;   //xyz
	XMFLOAT3 normal;//法線
	XMFLOAT2 uv;    //uv
};

//マテリアルデータ構造体
struct Material
{
	string   mtlName;      //マテリアル名(objファイルからではない場合、使わなくていい？)
	XMFLOAT3 ambient;      //アンビエント
	XMFLOAT3 diffuse;      //ディフューズ
	XMFLOAT3 specurar;     //スペキュラー
	float    alpha;        //アルファ
	string   texFileName;  //テクスチャ名
	UINT     texNumber;    //テクスチャ番号(使用する場合必ず必要)
	//コンストラクタ
	Material() {
		ambient = { 0.3f,0.3f,0.3f };
		diffuse = { 0.0f,0.0f,0.0f };
		specurar = { 0.0f,0.0f,0.0f };
		alpha = 1.0f;
	}
};

class NY_Object3DManager;

class NY_Model3D
{
public:
	//頂点データ
	vector<Vertex> vertices;
	//頂点インデックス
	vector<unsigned short> indices;
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデクスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibview{};
	//マテリアル
	Material material;

public:

	NY_Model3D() {};

	//----------ファイルを読み込んでモデルデータを作る関数群----------//

	//objファイル読み込み。読み込んだ後のバッファ関連の処理も行う。テクスチャが絡むため、Object3Dマネージャを引数で取得
	void LoadObjModel(const char *filename, ID3D12Device *dev, NY_Object3DManager *mgr);
	//マテリアル読み込み。LoadObjModel内で実行され、materialにデータを格納
	void LoadMatarial(string fileName,string filepath, ID3D12Device *dev, NY_Object3DManager *mgr);

	//----------モデルデータを1から作成する関数群----------//

	//xy平面を作成。テクスチャのロードが必須なため使いにくいかも
	//void CreatePlaneModelXY(float x_size, float y_size, string useTexFileName, ID3D12Device *dev, NY_Object3DManager *mgr);

	//xy平面を作成。ロード済テクスチャを使用。テクスチャバッファの番号を指定すること（空のバッファを指定するとあとで死ぬ）
	void CreatePlaneModelXY(float x_size, float y_size, float uv_x, float uv_y, UINT useTexNum, ID3D12Device *dev);
	//xz平面
	void CreatePlaneModelXZ(float x_size, float z_size, float uv_x, float uv_y, UINT useTexNum, ID3D12Device *dev);
	//yz平面
	void CreatePlaneModelYZ(float y_size, float z_size, float uv_x, float uv_y, UINT useTexNum, ID3D12Device *dev);

	//立方体モデルデータ作成
	void CreateBoxModel(float size, float uv_x, float uv_y, UINT useTexNum, ID3D12Device *dev);

	//object3Dで使用する頂点データを作成(製作中)
	//static SingleVertex GetSingleVertexData(ID3D12Device *dev);

private:

	//ジオメトリ用三角形生成（製作中）
	//void CreateTriangleGeometoryTest(UINT usetexnum, ID3D12Device *dev);

};

