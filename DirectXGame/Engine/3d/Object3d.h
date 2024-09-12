#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <string>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix44.h"
#include <vector>
#include "WorldTransform.h"
#include "Camera.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ModelManager.h"
#include "Animation.h"

class Object3d{
private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	enum class RootParameter {
		kMaterial,
		kWorldTransform,
		kCamera,
		kTexture,
		kEnvironmentTex,
		kDirectionLight,
		kPointLight,
		kSpotLight,
		kUtilsParameter,
		
		kParamNum,
	};

private: //静的メンバ変数

	static ID3D12Device* device_;
	static ID3D12GraphicsCommandList* commandList_;
	static ComPtr<ID3D12RootSignature> rootSignature_;
	static ComPtr<ID3D12PipelineState> graphicsPipelineState_;

public: //静的メンバ関数

	//静的初期化
	static void StaticInitialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	//モデルの生成
	static Object3d* Create(std::shared_ptr<Model> model);
	//描画前処理
	static void preDraw();
	//描画後処理
	static void postDraw();

	static void SetPointLight(PointLight* pointLight) { pointLight_ = pointLight; }

	static void SetSpotLight(SpotLight* spotLight) { spotLight_ = spotLight; }

public:

	static PointLight* pointLight_;
	static SpotLight* spotLight_;

	
private:

	//シェーダのコンパイル
	static ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandleer);
	
private: //メンバ変数

	std::shared_ptr<Model> model_;

public:

	WorldTransform worldTransform_;

public: //メンバ関数

	//初期化
	void Initialize(std::shared_ptr<Model> model);
	//更新
	void Update();
	//描画
	void Draw(const Camera& camera);
	void Draw(const Camera& camera, const uint32_t& textureHandle);

	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }
	
	void SetModelHandle(std::shared_ptr<Model> model) { model_ = model; }

	Vector3 GetWorldPos() const;
	std::shared_ptr<Model> GetModel() const { return model_; }

public: // 松井が追加する箇所
	class Utils {
	public:
		Utils();
		~Utils();

		ComPtr<ID3D12Resource> cBuffer_;
		struct UtilsParameter {
			Vector4 color_ = Vector4(1.0f, 1.0f, 1.0, 1.0f);
		};
		UtilsParameter* cUtilsParam_;

		void CreateConstantBuffer();
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
			return cBuffer_->GetGPUVirtualAddress();
		}
		void Map();
	};
	std::unique_ptr<Utils> utilsParam_;
	

	

};

