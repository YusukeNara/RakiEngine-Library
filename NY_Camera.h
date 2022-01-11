#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class NY_Camera
{
public:
	//ビュー変換情報
	XMFLOAT3 _eye;//視点
	XMFLOAT3 _target;//注視点
	XMFLOAT3 _up;//上ベクトル
	//ビュー行列
	XMMATRIX _matView;

	//アフィン変換情報
	XMFLOAT3 _scale{ 1,1,1 };
	XMFLOAT3 _rotation{ 0,0,0 };
	XMFLOAT3 _position{ 0,0,0 };
	//カメラローカル
	XMMATRIX _camRocal;
	//カメラワールド
	XMMATRIX _camWorld;

	//追従オブジェクトのワールド行列アドレス
	//NY_Object3D *_matFollowObject;

public:
	//コンストラクタ
	NY_Camera(XMFLOAT3 eye_, XMFLOAT3 target_, XMFLOAT3 up_);
	NY_Camera() {};

	//カメラが追従するオブジェクトの設定
	//void SetFollowingObject(NY_Object3D *followObjWorld) { _matFollowObject = followObjWorld; }

	//カメラの更新(全方向、全角度追従、挙動は要改善)
	void UpdateFollowing(XMMATRIX followTarget);


	//カメラ更新関数群（今はz方向,XYをどれくらい追従するかの指定も可能） 
	
	//Z軸座標のみ追従。追跡距離指定可能。XY方向への微追従もできる（挙動は強引）
	void UpdateFollowingOnlyZ(XMFLOAT3 followTargetPos,XMFLOAT3 followRange,float XfollowPower = 0, float YfollowPower = 0);
	//資料に合わせた追従カメラ。
	void UpdateFollowingOnlyZVer2(XMFLOAT3 followTarget,XMFLOAT3 followRange,float followRad);
	//さらに改変を加えた実験カメラ。違いは、ワールド変換を行い、ビュー行列を再定義すること
	void UpdateFollowingOnlyZWorldMat(XMMATRIX targetWorld);
	/*
	これまでの処理の問題点
	・World変換バージョンは、ビュー行列の構成要素自体は変わらないため、ビュー構成要素を用いた処理（ビルボード）が出来ない
	・ビュー変換バージョンは、ビュー構成要素による処理は可能だが、ワールド座標系での管理ができないので、カメラアングルの変更が不自由
	ではこれらを合わせた場合どうなるか？
	*/
	void UpdateFollowingViewAndWorld(XMMATRIX target, XMFLOAT3 range);
	void UpdateFollowingViewAndWorld(XMMATRIX target);

	void UpdateViewMat() {
		_matView = XMMatrixIdentity();
		_matView = XMMatrixLookAtLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_target), XMLoadFloat3(&_up));
	}
};

