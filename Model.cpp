#include "Model.h"
#include "Device.h"
Model::Model() 
{

}
Model::~Model()
{

}
void Model::initialize(XMFLOAT3 vertices[3], ID3D12Device* dev)
{
	//頂点データ
	for (int i = 0; i < 3; i++)
	{
		this->vertices[i] = vertices[i];
	}

	//インデックスデータ
	uint16_t indices[3] =
	{
		0,1,2,
	};
	//頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点の要素数
	sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(this->vertices));

	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの作成

	result = dev->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < _countof(this->vertices); i++)
	{
		vertMap[i] = vertices[i];//座標をコピー
	}

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファービューの作成
	
	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	//定数バッファ
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//色
	};

	//ヒープ設定
	
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	constBuffMaterial = nullptr;
	//定数バッファの生成
	result = dev->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,

		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);


	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t)) * _countof(this->indices);

	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	indexBuff = nullptr;
	result = dev->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//インデックスバッファをマッピング
	indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(this->indices); i++)
	{
		indexMap[i] = indices[i];
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの生成

	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

}

void Model::Update()
{

}
void Model::Draw(ID3D12GraphicsCommandList* commandList)
{
	//頂点バッファービューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbView);

	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	//インデックスバッファビューの設定コマンド
	commandList->IASetIndexBuffer(&ibView);

	//描画コマンド

	commandList->DrawIndexedInstanced(_countof(this->indices), 1, 0, 0, 0);//全ての頂点を使って描画
}