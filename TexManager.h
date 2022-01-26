#pragma once

#include <d3dx12.h>
#include <DirectXTex.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")

//マルチパスレンダリングをするときにあると今後が楽だと思って作った
class TexManager
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	TexManager() {};
	~TexManager() {};

	static ID3D12Device *dev;

public:
	//テクスチャ一枚のデータ
	struct texture {
		UINT									texNumber;
		Microsoft::WRL::ComPtr<ID3D12Resource>	texBuff;	//テクスチャバッファ
		const DirectX::Image					*img;		
		DirectX::TexMetadata					metaData;
		DirectX::ScratchImage					scratchImg;
		//アニメーション用uv管理
		std::vector<DirectX::XMFLOAT2>			uv_offsets;	//uv値オフセット配列
	};
	//テクスチャデータ配列
	static texture textureData[1024];
	//デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> texDsvHeap;
	//テクスチャ最大数
	static const int MAX_TEXNUM;

public:
	/// <summary>
	/// TexManagerの初期化、ディスクリプタヒープ生成
	/// </summary>
	static void InitTexManager();

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <param name="filename">const char型で画像ファイルの名前</param>
	/// <returns>格納したバッファの番号</returns>
	static UINT LoadTexture(const char *filename);

	static UINT LoadTexture(std::string filename);

	/// <summary>
	/// テクスチャの分割読み込み
	/// </summary>
	/// <param name="filename">画像ファイルの名前</param>
	/// <param name="numDivTex">分割数（右方向）</param>
	/// <param name="sizeX">1枚のサイズ</param>
	/// <returns>格納したテクスチャの場所を配列で</returns>
	static UINT LoadDivTextureTest(const char *filename, const int numDivTex, int sizeX);


};

