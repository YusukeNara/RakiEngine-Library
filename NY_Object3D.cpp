#include "NY_Object3D.h"
#include "TexManager.h"

//----- NY_Model3D -----//

//void NY_Model3D::LoadObjModel(const char *modelName, ID3D12Device *dev, NY_Object3DManager *mgr)
//{
//	//FileStream
//	ifstream file;
//	/*   モデル名のみで指定可能にするための処理（マテリアル読み込みを楽にするため）   */
//	string fileName = modelName;
//	string modelpath = "Resources/" + fileName;
//
//	//objファイルオープン
//	file.open(modelpath + ".obj");
//	//オープン失敗をチェック
//	if (file.fail())
//	{
//		assert(0);
//	}
//
//	//数値格納
//	vector<XMFLOAT3> positions;
//	vector<XMFLOAT3> normals;
//	vector<XMFLOAT2> texcoords;
//
//	//1行ずつ読み込み
//	string line;
//	while (getline(file, line)) {
//
//		//1行分の文字列をストリームに変換して読みやすくする
//		istringstream line_stream(line);
//
//		//半角スペースで行の先頭文字列取得
//		string key;
//		getline(line_stream, key, ' ');
//
//		//先頭が v => 頂点座標
//		if (key == "v") {
//			//xyz座標読み込み
//			XMFLOAT3 loadedPos{};
//			line_stream >> loadedPos.x;
//			line_stream >> loadedPos.y;
//			line_stream >> loadedPos.z;
//			//座標データ追加
//			positions.emplace_back(loadedPos);
//		}
//
//		//先頭が f => ポリゴンインデックス
//		if (key == "f") {
//			//半角スペース区切りで行の続きを読み込む
//			string index_string;
//			while (getline(line_stream, index_string, ' ')) {
//				//頂点インデックス1個分の文字列をストリームに変換して読みやすくする
//				istringstream index_stream(index_string);
//				unsigned short indexPosition, indexNormal, indexTexcoord;
//				index_stream >> indexPosition;
//				index_stream.seekg(1, ios_base::cur);//スラッシュ飛ばす
//				index_stream >> indexTexcoord;
//				index_stream.seekg(1, ios_base::cur);//スラッシュ飛ばす
//				index_stream >> indexNormal;
//				//頂点データ追加
//				Vertex vertex{};
//				vertex.pos = positions[indexPosition - 1];
//				vertex.normal = normals[indexNormal - 1];
//				vertex.uv = texcoords[indexTexcoord - 1];
//				vertices.emplace_back(vertex);
//				//頂点インデックスに追加
//				indices.emplace_back((unsigned short)indices.size());
//			}
//		}
//
//		//先頭が vt => テクスチャ
//		if (key == "vt") {
//			//U,V成分読み込み
//			XMFLOAT2 texcoord{};
//			line_stream >> texcoord.x;
//			line_stream >> texcoord.y;
//			//V方向反転
//			texcoord.y = 1.0f - texcoord.y;
//			//テクスチャ座標データに追加
//			texcoords.emplace_back(texcoord);
//		}
//
//		//先頭が vn => 法線ベクトル
//		if (key == "vn") {
//			//x,y,z成分読み込み
//			XMFLOAT3 normal{};
//			line_stream >> normal.x;
//			line_stream >> normal.y;
//			line_stream >> normal.z;
//			//法線ベクトルにデータ追加
//			normals.emplace_back(normal);
//		}
//
//		//先頭が mtllib => マテリアル
//		if (key == "mtllib") {
//			string filename;
//			line_stream >> filename;
//			LoadMatarial(filename, dev, mgr);
//		}
//
//	}
//	//----------以降は読み込んだ後の処理----------//
//
//	HRESULT result;
//
//	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
//	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
//
//	//頂点バッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff)
//	);
//	//頂点バッファビュー設定
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeVB;
//	vbView.StrideInBytes = sizeof(Vertex);
//
//	//インデックスバッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&indexBuff)
//	);
//	//インデックスバッファビュー設定
//	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
//	ibview.SizeInBytes = sizeIB;
//	ibview.Format = DXGI_FORMAT_R16_UINT;
//
//	//頂点バッファデータ転送
//	Vertex *vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
//	copy(vertices.begin(), vertices.end(), vertMap);
//	vertBuff->Unmap(0, nullptr);
//
//	//インデックスバッファデータ転送
//	unsigned short *indexMap = nullptr;
//	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
//	copy(indices.begin(), indices.end(), indexMap);
//	indexBuff->Unmap(0, nullptr);
//
//}
//
//void NY_Model3D::LoadMatarial(string path, ID3D12Device *dev, NY_Object3DManager *mgr)
//{
//	ifstream file;
//	//マテリアルファイルオープン
//	file.open("Resources/" + path);
//	//ファイルオープン失敗をチェック
//	if (file.fail()) {
//		assert(0);
//	}
//
//	//1行ずつ読み込む
//	string line;
//	while (getline(file, line))
//	{
//		//ストリーム変換
//		istringstream line_stream(line);
//		//半角スペース区切りで先頭文字列取得
//		string key;
//		getline(line_stream, key, ' ');
//		//先頭タブ文字は無視
//		if (key[0] == '\t') {
//			key.erase(key.begin());
//		}
//
//		//newmtl == マテリアル名
//		if (key == "newmtl") {
//			line_stream >> material.mtlName;
//		}
//
//		//Ka == アンビエント
//		if (key == "Ka") {
//			line_stream >> material.ambient.x;
//			line_stream >> material.ambient.y;
//			line_stream >> material.ambient.z;
//		}
//
//		//Kd == ディフューズ
//		if (key == "Kd") {
//			line_stream >> material.diffuse.x;
//			line_stream >> material.diffuse.y;
//			line_stream >> material.diffuse.z;
//		}
//
//		//Ks == スペキュラー
//		if (key == "Ks") {
//			line_stream >> material.specurar.x;
//			line_stream >> material.specurar.y;
//			line_stream >> material.specurar.z;
//		}
//
//		//map_Kd == テクスチャファイル名
//		if (key == "map_Kd") {
//			line_stream >> material.texFileName;
//			//テクスチャが格納されていないテクスチャバッファを探す
//			NY_Object3DManager::Get()->LoadObject3DTexture(material.texNumber, material.texFileName, dev);
//		}
//	}
//
//}
//
////void NY_Model3D::CreatePlaneModelXY(float x_size, float y_size, string useTexFileName, ID3D12Device *dev, NY_Object3DManager *mgr)
////{
////	//頂点データ作成
////	Vertex plane[4] = {
////		{{-x_size,-y_size,0},{},{0.0f,0.0f}},
////		{{ x_size,-y_size,0},{},{1.0f,0.0f}},
////		{{-x_size, y_size,0},{},{0.0f,1.0f}},
////		{{ x_size, y_size,0},{},{1.0f,1.0f}},
////	};
////	//頂点データ格納
////	for (int i = 0; i < 4; i++)
////	{
////		vertices.push_back(plane[i]);
////	}
////	//インデックスデータ作成
////	unsigned short index[] = {
////		0,1,2,
////		2,1,3,
////	};
////	//インデックスデータ格納
////	for (int i = 0; i < _countof(index); i++)
////	{
////		indices.push_back(index[i]);
////	}
////
////	HRESULT result;
////
////	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
////	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
////
////	//頂点バッファ生成
////	result = dev->CreateCommittedResource(
////		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
////		D3D12_HEAP_FLAG_NONE,
////		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
////		D3D12_RESOURCE_STATE_GENERIC_READ,
////		nullptr,
////		IID_PPV_ARGS(&vertBuff)
////	);
////	//頂点バッファビュー設定
////	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
////	vbView.SizeInBytes = sizeVB;
////	vbView.StrideInBytes = sizeof(Vertex);
////
////	//インデックスバッファ生成
////	result = dev->CreateCommittedResource(
////		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
////		D3D12_HEAP_FLAG_NONE,
////		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
////		D3D12_RESOURCE_STATE_GENERIC_READ,
////		nullptr,
////		IID_PPV_ARGS(&indexBuff)
////	);
////	//インデックスバッファビュー設定
////	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
////	ibview.SizeInBytes = sizeIB;
////	ibview.Format = DXGI_FORMAT_R16_UINT;
////
////	//頂点バッファデータ転送
////	Vertex *vertMap = nullptr;
////	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
////	copy(vertices.begin(), vertices.end(), vertMap);
////	vertBuff->Unmap(0, nullptr);
////
////	//インデックスバッファデータ転送
////	unsigned short *indexMap = nullptr;
////	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
////	copy(indices.begin(), indices.end(), indexMap);
////	indexBuff->Unmap(0, nullptr);
////
////	//テクスチャロード
////	//テクスチャが格納されていないテクスチャバッファを探す
////	for (int texpath = 1; texpath < _countof(mgr->texbuff); texpath++)
////	{
////		if (mgr->texbuff[texpath] == nullptr)//空のテクスチャバッファを発見
////		{
////			//Obj3DMgrのテクスチャ読み込み関数を実行
////			mgr->LoadObject3DTexture((UINT)texpath, useTexFileName, dev);
////			//texNumberにマテリアルでロードしたテクスチャのSRV番号を格納
////			material.texNumber = texpath;
////			//ロードしたらループを抜ける
////			break;
////		}
////	}
////
////}
//
//void NY_Model3D::CreatePlaneModelXY(float x_size, float y_size, float uv_x, float uv_y, UINT useTexNum, ID3D12Device *dev)
//{
//	//頂点データ作成
//	Vertex plane[4] = {
//		{{-x_size,-y_size,0},{},{0.0f,0.0f}},
//		{{ x_size,-y_size,0},{},{uv_x,0.0f}},
//		{{-x_size, y_size,0},{},{0.0f,uv_y}},
//		{{ x_size, y_size,0},{},{uv_x,uv_y}},
//	};
//	//頂点データ格納
//	for (int i = 0; i < 4; i++)
//	{
//		vertices.push_back(plane[i]);
//	}
//	//インデックスデータ作成
//	unsigned short index[] = {
//		0,2,1,
//		1,2,3,
//	};
//	//インデックスデータ格納
//	for (int i = 0; i < _countof(index); i++)
//	{
//		indices.push_back(index[i]);
//	}
//
//	HRESULT result;
//
//	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
//	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
//
//	//頂点バッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff)
//	);
//	//頂点バッファビュー設定
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeVB;
//	vbView.StrideInBytes = sizeof(Vertex);
//
//	//インデックスバッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&indexBuff)
//	);
//	//インデックスバッファビュー設定
//	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
//	ibview.SizeInBytes = sizeIB;
//	ibview.Format = DXGI_FORMAT_R16_UINT;
//
//	//頂点バッファデータ転送
//	Vertex *vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
//	copy(vertices.begin(), vertices.end(), vertMap);
//	vertBuff->Unmap(0, nullptr);
//
//	//インデックスバッファデータ転送
//	unsigned short *indexMap = nullptr;
//	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
//	copy(indices.begin(), indices.end(), indexMap);
//	indexBuff->Unmap(0, nullptr);
//
//	//マテリアルの番号設定
//	material.texNumber = useTexNum;
//
//}
//
//void NY_Model3D::CreatePlaneModelXZ(float x_size, float z_size, float uv_x, float uv_y, UINT useTexNum, ID3D12Device *dev)
//{
//	//頂点データ作成
//	Vertex plane[4] = {
//		{{-x_size,0,-z_size},{},{0.0f,0.0f}},
//		{{ x_size,0,-z_size},{},{uv_x,0.0f}},
//		{{-x_size,0, z_size},{},{0.0f,uv_y}},
//		{{ x_size,0, z_size},{},{uv_x,uv_y}},
//	};
//	//頂点データ格納
//	for (int i = 0; i < 4; i++)
//	{
//		vertices.push_back(plane[i]);
//	}
//	//インデックスデータ作成
//	unsigned short index[] = {
//		0,2,1,
//		1,2,3,
//	};
//	//インデックスデータ格納
//	for (int i = 0; i < _countof(index); i++)
//	{
//		indices.push_back(index[i]);
//	}
//
//	HRESULT result;
//
//	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
//	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
//
//	//頂点バッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff)
//	);
//	//頂点バッファビュー設定
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeVB;
//	vbView.StrideInBytes = sizeof(Vertex);
//
//	//インデックスバッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&indexBuff)
//	);
//	//インデックスバッファビュー設定
//	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
//	ibview.SizeInBytes = sizeIB;
//	ibview.Format = DXGI_FORMAT_R16_UINT;
//
//	//頂点バッファデータ転送
//	Vertex *vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
//	copy(vertices.begin(), vertices.end(), vertMap);
//	vertBuff->Unmap(0, nullptr);
//
//	//インデックスバッファデータ転送
//	unsigned short *indexMap = nullptr;
//	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
//	copy(indices.begin(), indices.end(), indexMap);
//	indexBuff->Unmap(0, nullptr);
//
//	//マテリアルの番号設定
//	material.texNumber = useTexNum;
//}
//
//void NY_Model3D::CreatePlaneModelYZ(float y_size, float z_size, float uv_x, float uv_y, UINT useTexNum, ID3D12Device *dev)
//{
//	//頂点データ作成
//	Vertex plane[4] = {
//		{{ 0,-y_size,-z_size},{},{0.0f,0.0f}},
//		{{ 0, y_size,-z_size},{},{uv_x,0.0f}},
//		{{ 0,-y_size, z_size},{},{0.0f,uv_y}},
//		{{ 0, y_size, z_size},{},{uv_x,uv_y}},
//	};
//	//頂点データ格納
//	for (int i = 0; i < 4; i++)
//	{
//		vertices.push_back(plane[i]);
//	}
//	//インデックスデータ作成
//	unsigned short index[] = {
//		0,2,1,
//		1,2,3,
//	};
//	//インデックスデータ格納
//	for (int i = 0; i < _countof(index); i++)
//	{
//		indices.push_back(index[i]);
//	}
//
//	HRESULT result;
//
//	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
//	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
//
//	//頂点バッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff)
//	);
//	//頂点バッファビュー設定
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeVB;
//	vbView.StrideInBytes = sizeof(Vertex);
//
//	//インデックスバッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&indexBuff)
//	);
//	//インデックスバッファビュー設定
//	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
//	ibview.SizeInBytes = sizeIB;
//	ibview.Format = DXGI_FORMAT_R16_UINT;
//
//	//頂点バッファデータ転送
//	Vertex *vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
//	copy(vertices.begin(), vertices.end(), vertMap);
//	vertBuff->Unmap(0, nullptr);
//
//	//インデックスバッファデータ転送
//	unsigned short *indexMap = nullptr;
//	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
//	copy(indices.begin(), indices.end(), indexMap);
//	indexBuff->Unmap(0, nullptr);
//
//	//マテリアルの番号設定
//	material.texNumber = useTexNum;
//}
//
//void NY_Model3D::CreateBoxModel(float size, float uv_x, float uv_y, UINT useTexNum, ID3D12Device *dev)
//{
//	//頂点データ作成
//	Vertex box[] = {
//		//後
//		{{-size,-size,-size},{}, {0.0f,1.0f}},//左下
//		{{-size, size,-size},{}, {0.0f,0.0f}},//左上
//		{{ size,-size,-size},{}, {1.0f,1.0f}},//右下
//		{{ size, size,-size},{}, {1.0f,0.0f}},//右上
//		//前
//		{{-size,-size, size},{},{0.0f,1.0f}},//左下
//		{{-size, size, size},{},{0.0f,0.0f}},//左上
//		{{ size,-size, size},{},{1.0f,1.0f}},//右下
//		{{ size, size, size},{},{1.0f,0.0f}},//右上
//		//左
//		{{-size,-size,-size},{},{0.0f,1.0f}},//左下
//		{{-size,-size, size},{},{0.0f,0.0f}},//左上
//		{{-size, size,-size},{},{1.0f,1.0f}},//右下
//		{{-size, size, size},{},{1.0f,0.0f}},//右上
//		//右
//		{{ size,-size,-size},{},{0.0f,1.0f}},//左下
//		{{ size,-size, size},{},{0.0f,0.0f}},//左上
//		{{ size, size,-size},{},{1.0f,1.0f}},//右下
//		{{ size, size, size},{},{1.0f,0.0f}},//右上
//		//上
//		{{-size, size,-size},{},{1.0f,1.0f}},//右下
//		{{-size, size, size},{},{1.0f,0.0f}},//右上
//		{{ size, size,-size},{},{0.0f,1.0f}},//左下
//		{{ size, size, size},{},{0.0f,0.0f}},//左上
//		//下
//		{{ size,-size,-size},{},{0.0f,1.0f}},//左下
//		{{ size,-size, size},{},{0.0f,0.0f}},//左上
//		{{-size,-size,-size},{},{1.0f,1.0f}},//右下
//		{{-size,-size, size},{},{1.0f,0.0f}},//右上
//
//	};
//	//頂点データ格納
//	for (int i = 0; i < _countof(box); i++)
//	{
//		vertices.push_back(box[i]);
//	}
//	//インデックスデータ作成
//	unsigned short index[] = {
//		//前
//		0,1,2,
//		2,1,3,
//		//後
//		7,5,6,
//		6,5,4,
//		//左
//		8,9,10,
//		10,9,11,
//		//右
//		14,15,12,
//		12,15,13,
//		//上
//		16,17,18,
//		18,17,19,
//		//下
//		20,21,22,
//		22,21,23,
//	};
//	//インデックスデータ格納
//	for (int i = 0; i < _countof(index); i++)
//	{
//		indices.push_back(index[i]);
//	}
//
//	HRESULT result;
//
//	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());
//	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());
//
//	//頂点バッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff)
//	);
//	//頂点バッファビュー設定
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeVB;
//	vbView.StrideInBytes = sizeof(Vertex);
//
//	//インデックスバッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&indexBuff)
//	);
//	//インデックスバッファビュー設定
//	ibview.BufferLocation = indexBuff->GetGPUVirtualAddress();
//	ibview.SizeInBytes = sizeIB;
//	ibview.Format = DXGI_FORMAT_R16_UINT;
//
//	//頂点バッファデータ転送
//	Vertex *vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
//	copy(vertices.begin(), vertices.end(), vertMap);
//	vertBuff->Unmap(0, nullptr);
//
//	//インデックスバッファデータ転送
//	unsigned short *indexMap = nullptr;
//	result = indexBuff->Map(0, nullptr, (void **)&indexMap);
//	copy(indices.begin(), indices.end(), indexMap);
//	indexBuff->Unmap(0, nullptr);
//
//	//マテリアルの番号設定
//	material.texNumber = useTexNum;
//}
//
//
//
//void NY_Model3D::CreateTriangleGeometoryTest(UINT usetexnum,ID3D12Device *dev)
//{
//	//頂点データ作成
//	ParticleVertex plane[] = {
//		{{0.0f,0.0f,0.0f}},
//	};
//	//頂点データ格納
//	for (int i = 0; i < 4; i++)
//	{
//		pvert.push_back(plane[i]);
//	}
//
//	HRESULT result;
//
//	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * pvert.size());
//
//	//頂点バッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff)
//	);
//
//	//頂点バッファビュー設定
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	vbView.SizeInBytes = sizeVB;
//	vbView.StrideInBytes = sizeof(Vertex);
//
//	//頂点バッファデータ転送
//	ParticleVertex *vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
//	copy(pvert.begin(), pvert.end(), vertMap);
//	vertBuff->Unmap(0, nullptr);
//
//	//テクスチャロード
//	material.texNumber = usetexnum;
//
//
//}
//
//SingleVertex NY_Model3D::GetSingleVertexData(ID3D12Device *dev)
//{
//	SingleVertex rv;
//
//	rv.v = { {0,0,0},{},{0,0} };
//
//	HRESULT result;
//
//	UINT sizeVB = static_cast<UINT>(sizeof(Vertex));
//
//	//頂点バッファ生成
//	result = dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&rv.vb)
//	);
//
//	//頂点バッファビュー設定
//	rv.vbview.BufferLocation = rv.vb->GetGPUVirtualAddress();
//	rv.vbview.SizeInBytes = sizeVB;
//	rv.vbview.StrideInBytes = sizeof(Vertex);
//
//	//頂点バッファデータ転送
//	Vertex *vertMap = nullptr;
//	result = rv.vb -> Map(0, nullptr, (void **)&vertMap);
//	vertMap = &rv.v;
//	rv.vb -> Unmap(0, nullptr);
//
//	return rv;
//}

//----- NY_Object3D -----//

void Object3d::InitObject3D(Object3d *obj, ID3D12Device *dev)
{
	HRESULT result;
	const auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);

	//定数バッファb0生成
	result = dev->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&obj->constBuffB0)
	);

	//定数バッファb1生成
	result = dev->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&obj->constBuffB1)
	);

}

void Object3d::InitObject3D(ID3D12Device *dev)
{
	HRESULT result;
	const auto HEAP_PROP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const auto RESDESC = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB0) + 0xff) & ~0xff);

	//定数バッファb0生成
	result = dev->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0)
	);

	//定数バッファb1生成
	result = dev->CreateCommittedResource(
		&HEAP_PROP,
		D3D12_HEAP_FLAG_NONE,
		&RESDESC,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB1)
	);
}

void Object3d::SetLoadedModelData(Object3d *obj, NY_Model3D *loadedModel)
{
	obj->model = loadedModel;
}

void Object3d::SetLoadedModelData(NY_Model3D *loadedModel)
{
	model = loadedModel;

	isDirty = true;
}

void Object3d::UpdateObject3D(Object3d *obj, XMMATRIX &matview)
{
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行行列の計算

	matScale = XMMatrixScaling(obj->scale.x, obj->scale.y, obj->scale.z);

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(obj->rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(obj->rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(obj->rotation.y));

	matTrans = XMMatrixTranslation(obj->position.x, obj->position.y, obj->position.z);

	obj->matWorld = XMMatrixIdentity();
	obj->matWorld *= matScale;//ワールド行列にスケーリングを反映
	obj->matWorld *= matRot;
	obj->matWorld *= matTrans;

	if (obj->parent != nullptr)
	{
		obj->matWorld *= obj->parent->matWorld;
	}

	//定数バッファB0データ転送
	ConstBufferDataB0 *ConstMapB0 = nullptr;
	if (SUCCEEDED(obj->constBuffB0->Map(0, nullptr, (void **)&ConstMapB0)))
	{
		ConstMapB0->mat = obj->matWorld * matview * NY_Object3DManager::Get()->matProjection;
		obj->constBuffB0->Unmap(0, nullptr);
	}

	//定数バッファB1データ転送
	ConstBufferDataB1 *ConstMapB1 = nullptr;
	if (SUCCEEDED(obj->constBuffB1->Map(0, nullptr, (void **)&ConstMapB1)))
	{
		ConstMapB1->amdient  = obj->model->material.ambient;
		ConstMapB1->diffuse  = obj->model->material.diffuse;
		ConstMapB1->specular = obj->model->material.specurar;
		ConstMapB1->alpha    = obj->model->material.alpha;
		obj->constBuffB1->Unmap(0, nullptr);
	}

}

void Object3d::UpdateObject3D(NY_Camera *matview)
{
	//ダーティフラグの挙動は検討中
	isDirty = true;

	//ダーティフラグが有効な時更新（通常オブジェクト）
	if (isDirty && isBillBoard != true) {

		XMMATRIX matScale, matRot, matTrans;

		// スケール、回転、平行行列の計算

		matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
		matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
		matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

		matTrans = XMMatrixTranslation(position.x, position.y, position.z);

		matWorld = XMMatrixIdentity();
		matWorld *= matScale;//ワールド行列にスケーリングを反映
		matWorld *= matRot;
		matWorld *= matTrans;

		if (parent != nullptr)
		{
			matWorld *= parent->matWorld;
		}

		//定数バッファB0データ転送
		ConstBufferDataB0 *ConstMapB0 = nullptr;
		if (SUCCEEDED(constBuffB0->Map(0, nullptr, (void **)&ConstMapB0)))
		{
			ConstMapB0->mat = matWorld * matview->_matView * NY_Object3DManager::Get()->matProjection;
			constBuffB0->Unmap(0, nullptr);
		}

		//定数バッファB1データ転送
		ConstBufferDataB1 *ConstMapB1 = nullptr;
		if (SUCCEEDED(constBuffB1->Map(0, nullptr, (void **)&ConstMapB1)))
		{
			ConstMapB1->amdient = model->material.ambient;
			ConstMapB1->diffuse = model->material.diffuse;
			ConstMapB1->specular = model->material.specurar;
			ConstMapB1->alpha = model->material.alpha;
			constBuffB1->Unmap(0, nullptr);
		}

		//ダーティフラグリセット
		isDirty = false;
	}
	//ビルボードのとき
	else if (isDirty && isBillBoard) {
		UpdateBillBoard3D(matview);

		isDirty = false;
	}
}

void Object3d::UpdateBillBoard3D(Object3d *obj, NY_Camera cam)
{
	/*
	ビルボードの原理は、ビューで回す方向の反対を予めかけりゃええやん！だが
	NY_Cameraクラスの欠点として、Camera本体のワールド座標は一切替わらないので
	NY_Cameraのインスタンスでビルボードの更新は出来ない。
	よって、Cameraの「追従するターゲット」を用いて逆回転を求める必要がある?
	*/


	////ビルボード用変換行列を宣言
	XMMATRIX billBoardMat;
	billBoardMat = XMMatrixIdentity();

	billBoardMat = XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, -XMVector3Normalize(XMLoadFloat3(&cam._eye) - XMLoadFloat3(&cam._target)), XMLoadFloat3(&cam._up));
	billBoardMat = XMMatrixInverse(nullptr, billBoardMat);

	//ワールド変換用
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行行列の計算

	matScale = XMMatrixScaling(obj->scale.x, obj->scale.y, obj->scale.z);

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(obj->rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(obj->rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(obj->rotation.y));

	matTrans = XMMatrixTranslation(obj->position.x, obj->position.y, obj->position.z);

	obj->matWorld = XMMatrixIdentity();

	obj->matWorld *= billBoardMat;

	obj->matWorld *= matScale;
	obj->matWorld *= matRot;
	obj->matWorld *= matTrans;

	if (obj->parent != nullptr)
	{
		obj->matWorld *= obj->parent->matWorld;
	}

	//定数バッファB0データ転送
	ConstBufferDataB0 *ConstMapB0 = nullptr;
	if (SUCCEEDED(obj->constBuffB0->Map(0, nullptr, (void **)&ConstMapB0)))
	{
		ConstMapB0->mat = obj->matWorld * cam._matView * NY_Object3DManager::Get()->matProjection;
		obj->constBuffB0->Unmap(0, nullptr);
	}

	//定数バッファB1データ転送
	ConstBufferDataB1 *ConstMapB1 = nullptr;
	if (SUCCEEDED(obj->constBuffB1->Map(0, nullptr, (void **)&ConstMapB1)))
	{
		ConstMapB1->amdient  = obj->model->material.ambient;
		ConstMapB1->diffuse  = obj->model->material.diffuse;
		ConstMapB1->specular = obj->model->material.specurar;
		ConstMapB1->alpha    = obj->model->material.alpha;
		obj->constBuffB1->Unmap(0, nullptr);
	}

}

void Object3d::UpdateBillBoard3D(NY_Camera *cam)
{
	/*
	ビルボードの原理は、ビューで回す方向の反対を予めかけりゃええやん！だが
	NY_Cameraクラスの欠点として、Camera本体のワールド座標は一切替わらないので
	NY_Cameraのインスタンスでビルボードの更新は出来ない。
	よって、Cameraの「追従するターゲット」を用いて逆回転を求める必要がある?
	*/


////ビルボード用変換行列を宣言
	XMMATRIX billBoardMat;
	billBoardMat = XMMatrixIdentity();

	billBoardMat = XMMatrixLookAtLH({ 0.0f,0.0f,0.0f }, -XMVector3Normalize(XMLoadFloat3(&cam->_eye) - XMLoadFloat3(&cam->_target)), XMLoadFloat3(&cam->_up));
	billBoardMat = XMMatrixInverse(nullptr, billBoardMat);

	//ワールド変換用
	XMMATRIX matScale, matRot, matTrans;

	// スケール、回転、平行行列の計算

	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));

	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity();

	matWorld *= billBoardMat;

	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	if (parent != nullptr)
	{
		matWorld *= parent->matWorld;
	}

	//定数バッファB0データ転送
	ConstBufferDataB0 *ConstMapB0 = nullptr;
	if (SUCCEEDED(constBuffB0->Map(0, nullptr, (void **)&ConstMapB0)))
	{
		ConstMapB0->mat = matWorld * cam->_matView * NY_Object3DManager::Get()->matProjection;
		constBuffB0->Unmap(0, nullptr);
	}

	//定数バッファB1データ転送
	ConstBufferDataB1 *ConstMapB1 = nullptr;
	if (SUCCEEDED(constBuffB1->Map(0, nullptr, (void **)&ConstMapB1)))
	{
		ConstMapB1->amdient		= model->material.ambient;
		ConstMapB1->diffuse		= model->material.diffuse;
		ConstMapB1->specular	= model->material.specurar;
		ConstMapB1->alpha		= model->material.alpha;
		constBuffB1->Unmap(0, nullptr);
	}
}

void Object3d::DrawModel3D(Object3d *obj, ID3D12GraphicsCommandList *cmd, ID3D12Device *dev)
{
	//頂点バッファ設定
	cmd->IASetVertexBuffers(0, 1, &obj->model->vbView);
	//インデックスバッファ設定
	cmd->IASetIndexBuffer(&obj->model->ibview);
	//定数バッファ設定
	cmd->SetGraphicsRootConstantBufferView(0, obj->constBuffB0->GetGPUVirtualAddress());
	//定数バッファ設定
	cmd->SetGraphicsRootConstantBufferView(1, obj->constBuffB1->GetGPUVirtualAddress());

	//シェーダーリソースビューをセット
	cmd->SetGraphicsRootDescriptorTable(2,
	CD3DX12_GPU_DESCRIPTOR_HANDLE(NY_Object3DManager::Get()->descheap->GetGPUDescriptorHandleForHeapStart(),
	obj->model->material.texNumber, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//描画
	cmd->DrawIndexedInstanced(obj->model->indices.size(), 1, 0, 0, 0);
}

void Object3d::DrawModel3D(ID3D12GraphicsCommandList *cmd, ID3D12Device *dev)
{
	//頂点バッファ設定
	cmd->IASetVertexBuffers(0, 1, &model->vbView);
	//インデックスバッファ設定
	cmd->IASetIndexBuffer(&model->ibview);
	//定数バッファ設定
	cmd->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());
	//定数バッファ設定
	cmd->SetGraphicsRootConstantBufferView(1, constBuffB1->GetGPUVirtualAddress());

	//シェーダーリソースビューをセット
	cmd->SetGraphicsRootDescriptorTable(2,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(TexManager::texDsvHeap.Get()->GetGPUDescriptorHandleForHeapStart(),
			model->material.texNumber, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//描画
	cmd->DrawIndexedInstanced(model->indices.size(), 1, 0, 0, 0);
}

