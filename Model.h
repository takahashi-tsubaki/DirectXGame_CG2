#pragma once
#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <wrl.h>

using namespace DirectX;

//リンクの設定
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")//directInputのバージョン
#pragma comment(lib,"dxguid.lib")

class Model
{
public:
	Model();
	~Model();
	//関数
	void initialize(XMFLOAT3 vertices[3], ID3D12Device* dev);

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList);

	void CreateBuffer(HRESULT result, ID3D12Device* dev);

private:
	//変数

	HRESULT result;

	XMFLOAT3 vertices[3];

	uint16_t indices[3];
	
	ID3D12Device* dev;

	UINT sizeVB;

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};//ヒープ設定

	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};

	//頂点バッファの作成
	ID3D12Resource* vertBuff;

	XMFLOAT3* vertMap;

	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	
	//定数バッファ
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//色
	};

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};

	ID3D12Resource* constBuffMaterial;

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial;

	//ルートパラメータ
	D3D12_ROOT_PARAMETER rootParam = {};

	//インデックスバッファの生成
	ID3D12Resource* indexBuff;

	//インデックスバッファをマッピング
	uint16_t* indexMap;

	//インデックスバッファビューの生成
	D3D12_INDEX_BUFFER_VIEW ibView{};

	ID3D12Resource* constBuffTransform = nullptr;

	struct ConstBufferDataTransform
	{
		XMMATRIX mat;
	};

	ConstBufferDataTransform* constMapTransform = nullptr;

};