#pragma once
#include "Vec3.h"
#include "Matrix44.h"
#include <d3d12.h>
#include <wrl.h>

struct CBufferDataWorldTransform {
	Matrix4x4 matWorld;
	Matrix4x4 WorldInverseTranspose;
};

class WorldTransform{
public:

	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	
	Vector3 rotation_ = {};

	Vector3 translation_ = {};

	Matrix4x4 matWorld_ = MakeIdentity44();
	Matrix4x4 WorldInverseTransposeMat_ = MakeIdentity44();

	const WorldTransform* parent_ = nullptr;

	// world座標の取得
	Vector3 GetWorldPosition() const {
		return Vector3(matWorld_.m[3][0], matWorld_.m[3][1], matWorld_.m[3][2]);
	}

public:

	void Init();
	void UpdateMatrix();
	void UpdateMatrixRotate(const Matrix4x4& rotateMat);
	void Map();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return cBuffer_->GetGPUVirtualAddress();
	}

private:

	
	void CreateCBuffer();

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> cBuffer_;
	CBufferDataWorldTransform* cMap_ = nullptr;

};

