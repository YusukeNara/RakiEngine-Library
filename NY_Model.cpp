#include "NY_Model.h"
#include "TexManager.h"

void NY_Model3D::LoadObjModel(const char *modelName)
{
	//FileStream
	ifstream file;
	/*   モデル名のみで指定可能にするための処理（マテリアル読み込みを楽にするため）   */
	string fileName		= modelName;
	string folderName	= fileName + "/";
	string modelpath	= "Resources/objFiles/" + folderName + fileName;
	string matarialpath = "Resources/objFiles/" + folderName;

	//objファイルオープン
	file.open(modelpath + ".obj");
	//オープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	//数値格納
	vector<XMFLOAT3> positions;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> texcoords;

	//1行ずつ読み込み
	string line;
	while (getline(file, line)) {

		//1行分の文字列をストリームに変換して読みやすくする
		istringstream line_stream(line);

		//半角スペースで行の先頭文字列取得
		string key;
		getline(line_stream, key, ' ');

		//先頭が v => 頂点座標
		if (key == "v") {
			//xyz座標読み込み
			XMFLOAT3 loadedPos{};
			line_stream >> loadedPos.x;
			line_stream >> loadedPos.y;
			line_stream >> loadedPos.z;
			//座標データ追加
			positions.emplace_back(loadedPos);
		}

		//先頭が f => ポリゴンインデックス
		if (key == "f") {
			//半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				//頂点インデックス1個分の文字列をストリームに変換して読みやすくする
				istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, ios_base::cur);//スラッシュ飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);//スラッシュ飛ばす
				index_stream >> indexNormal;
				//頂点データ追加
				Vertex vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				vertices.emplace_back(vertex);
				//頂点インデックスに追加
				indices.emplace_back((unsigned short)indices.size());
			}
		}

		//先頭が vt => テクスチャ
		if (key == "vt") {
			//U,V成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}

		//先頭が vn => 法線ベクトル
		if (key == "vn") {
			//x,y,z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//法線ベクトルにデータ追加
			normals.emplace_back(normal);
		}

		//先頭が mtllib => マテリアル
		if (key == "mtllib") {
			string filename;
			line_stream >> filename;
			LoadMatarial(filename, matarialpath);
		}

	}
	//----------以降は読み込んだ後の処理----------//

	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	//一時オブジェクト
	const auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	//頂点バッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	//頂点バッファビュー設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
	//インデックスバッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);
	//インデックスバッファビュー設定
	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibview.SizeInBytes = sizeIB;
	ibview.Format = DXGI_FORMAT_R16_UINT;

	//頂点バッファデータ転送
	Vertex *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	copy(vertices.begin(), vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//インデックスバッファデータ転送
	unsigned short *indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	copy(indices.begin(), indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

}

void NY_Model3D::LoadMatarial(string fileName, string filepath)
{
	ifstream file;
	//マテリアルファイルオープン
	file.open(filepath + fileName);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	//1行ずつ読み込む
	string line;
	while (getline(file, line))
	{
		//ストリーム変換
		istringstream line_stream(line);
		//半角スペース区切りで先頭文字列取得
		string key;
		getline(line_stream, key, ' ');
		//先頭タブ文字は無視
		if (key[0] == '\t') {
			key.erase(key.begin());
		}

		//newmtl == マテリアル名
		if (key == "newmtl") {
			line_stream >> material.mtlName;
		}

		//Ka == アンビエント
		if (key == "Ka") {
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}

		//Kd == ディフューズ
		if (key == "Kd") {
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}

		//Ks == スペキュラー
		if (key == "Ks") {
			line_stream >> material.specurar.x;
			line_stream >> material.specurar.y;
			line_stream >> material.specurar.z;
		}

		//map_Kd == テクスチャファイル名
		if (key == "map_Kd") {
			line_stream >> material.texFileName;
			//テクスチャのロード
			material.texNumber = TexManager::LoadTexture(filepath + material.texFileName);
		}
	}

}

void NY_Model3D::CreatePlaneModelXY(float x_size, float y_size, float uv_x, float uv_y, UINT useTexNum)
{
	//頂点データ作成
	Vertex plane[4] = {
		{{-x_size,-y_size,0},{},{0.0f,0.0f}},
		{{ x_size,-y_size,0},{},{uv_x,0.0f}},
		{{-x_size, y_size,0},{},{0.0f,uv_y}},
		{{ x_size, y_size,0},{},{uv_x,uv_y}},
	};
	//頂点データ格納
	for (int i = 0; i < 4; i++)
	{
		vertices.push_back(plane[i]);
	}
	//インデックスデータ作成
	unsigned short index[] = {
		0,2,1,
		1,2,3,
	};
	//インデックスデータ格納
	for (int i = 0; i < _countof(index); i++)
	{
		indices.push_back(index[i]);
	}

	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	const auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	//頂点バッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	//頂点バッファビュー設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
	//インデックスバッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);
	//インデックスバッファビュー設定
	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibview.SizeInBytes = sizeIB;
	ibview.Format = DXGI_FORMAT_R16_UINT;

	//頂点バッファデータ転送
	Vertex *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	copy(vertices.begin(), vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//インデックスバッファデータ転送
	unsigned short *indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	copy(indices.begin(), indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//マテリアルの番号設定
	material.texNumber = useTexNum;

}

void NY_Model3D::CreatePlaneModelXZ(float x_size, float z_size, float uv_x, float uv_y, UINT useTexNum)
{
	//頂点データ作成
	Vertex plane[4] = {
		{{-x_size,0,-z_size},{},{0.0f,0.0f}},
		{{ x_size,0,-z_size},{},{uv_x,0.0f}},
		{{-x_size,0, z_size},{},{0.0f,uv_y}},
		{{ x_size,0, z_size},{},{uv_x,uv_y}},
	};
	//頂点データ格納
	for (int i = 0; i < 4; i++)
	{
		vertices.push_back(plane[i]);
	}
	//インデックスデータ作成
	unsigned short index[] = {
		0,2,1,
		1,2,3,
	};
	//インデックスデータ格納
	for (int i = 0; i < _countof(index); i++)
	{
		indices.push_back(index[i]);
	}

	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	const auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	//頂点バッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	//頂点バッファビュー設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
	//インデックスバッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);
	//インデックスバッファビュー設定
	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibview.SizeInBytes = sizeIB;
	ibview.Format = DXGI_FORMAT_R16_UINT;

	//頂点バッファデータ転送
	Vertex *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	copy(vertices.begin(), vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//インデックスバッファデータ転送
	unsigned short *indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	copy(indices.begin(), indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//マテリアルの番号設定
	material.texNumber = useTexNum;
}

void NY_Model3D::CreatePlaneModelYZ(float y_size, float z_size, float uv_x, float uv_y, UINT useTexNum)
{
	//頂点データ作成
	Vertex plane[4] = {
		{{ 0,-y_size,-z_size},{},{0.0f,0.0f}},
		{{ 0, y_size,-z_size},{},{uv_x,0.0f}},
		{{ 0,-y_size, z_size},{},{0.0f,uv_y}},
		{{ 0, y_size, z_size},{},{uv_x,uv_y}},
	};
	//頂点データ格納
	for (int i = 0; i < 4; i++)
	{
		vertices.push_back(plane[i]);
	}
	//インデックスデータ作成
	unsigned short index[] = {
		0,2,1,
		1,2,3,
	};
	//インデックスデータ格納
	for (int i = 0; i < _countof(index); i++)
	{
		indices.push_back(index[i]);
	}

	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	const auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	//頂点バッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	//頂点バッファビュー設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
	//インデックスバッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);
	//インデックスバッファビュー設定
	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibview.SizeInBytes = sizeIB;
	ibview.Format = DXGI_FORMAT_R16_UINT;

	//頂点バッファデータ転送
	Vertex *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	copy(vertices.begin(), vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//インデックスバッファデータ転送
	unsigned short *indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	copy(indices.begin(), indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//マテリアルの番号設定
	material.texNumber = useTexNum;
}

void NY_Model3D::CreateBoxModel(float size, float uv_x, float uv_y, UINT useTexNum)
{

	//頂点データ作成
	Vertex box[] = {
		//後
		{{-size,-size,-size},{0,0,1}, {0.0f,1.0f}},//左下
		{{-size, size,-size},{0,0,1}, {0.0f,0.0f}},//左上
		{{ size,-size,-size},{0,0,1}, {1.0f,1.0f}},//右下
		{{ size, size,-size},{0,0,1}, {1.0f,0.0f}},//右上
		//前
		{{-size,-size, size},{0,0,-1},{0.0f,1.0f}},//左下
		{{-size, size, size},{0,0,-1},{0.0f,0.0f}},//左上
		{{ size,-size, size},{0,0,-1},{1.0f,1.0f}},//右下
		{{ size, size, size},{0,0,-1},{1.0f,0.0f}},//右上
		//左
		{{-size,-size,-size},{-1,0,0},{0.0f,1.0f}},//左下
		{{-size,-size, size},{-1,0,0},{0.0f,0.0f}},//左上
		{{-size, size,-size},{-1,0,0},{1.0f,1.0f}},//右下
		{{-size, size, size},{-1,0,0},{1.0f,0.0f}},//右上
		//右
		{{ size,-size,-size},{1,0,0},{0.0f,1.0f}},//左下
		{{ size,-size, size},{1,0,0},{0.0f,0.0f}},//左上
		{{ size, size,-size},{1,0,0},{1.0f,1.0f}},//右下
		{{ size, size, size},{1,0,0},{1.0f,0.0f}},//右上
		//上
		{{-size, size,-size},{0,1,0},{1.0f,1.0f}},//右下
		{{-size, size, size},{0,1,0},{1.0f,0.0f}},//右上
		{{ size, size,-size},{0,1,0},{0.0f,1.0f}},//左下
		{{ size, size, size},{0,1,0},{0.0f,0.0f}},//左上
		//下
		{{ size,-size,-size},{0,-1,0},{0.0f,1.0f}},//左下
		{{ size,-size, size},{0,-1,0},{0.0f,0.0f}},//左上
		{{-size,-size,-size},{0,-1,0},{1.0f,1.0f}},//右下
		{{-size,-size, size},{0,-1,0},{1.0f,0.0f}},//右上

	};
	//頂点データ格納
	for (int i = 0; i < _countof(box); i++)
	{
		vertices.push_back(box[i]);
	}
	//インデックスデータ作成
	unsigned short index[] = {
		//前
		0,1,2,
		2,1,3,
		//後
		7,5,6,
		6,5,4,
		//左
		8,9,10,
		10,9,11,
		//右
		14,15,12,
		12,15,13,
		//上
		16,17,18,
		18,17,19,
		//下
		20,21,22,
		22,21,23,
	};
	//インデックスデータ格納
	for (int i = 0; i < _countof(index); i++)
	{
		indices.push_back(index[i]);
	}

	HRESULT result;

	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	const auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	//頂点バッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	//頂点バッファビュー設定
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	RESDESC = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
	//インデックスバッファ生成
	result = NY_Object3DManager::GetDev()->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);
	//インデックスバッファビュー設定
	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibview.SizeInBytes = sizeIB;
	ibview.Format = DXGI_FORMAT_R16_UINT;

	//頂点バッファデータ転送
	Vertex *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	copy(vertices.begin(), vertices.end(), vertMap);
	vertBuff->Unmap(0, nullptr);

	//インデックスバッファデータ転送
	unsigned short *indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
	copy(indices.begin(), indices.end(), indexMap);
	indexBuff->Unmap(0, nullptr);

	//マテリアルの番号設定
	material.texNumber = useTexNum;
}

void NY_Model3D::ChangeTexAnimationNumber(int useAnimNum)
{
	//範囲外参照を検知
	if (useAnimNum < 0) { return; }
	if (useAnimNum > TexManager::textureData[material.texNumber].uv_offsets.size()) { return; }

	int vposNumber = 0;
	//UV値を該当オフセットの値に変更
	for (int i = 0; i < vertices.size(); i++) {



		vposNumber++;
		if (vposNumber >= 3) {
			vposNumber == 0;
		}
	}


}
