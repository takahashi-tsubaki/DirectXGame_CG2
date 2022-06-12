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

//�����N�̐ݒ�
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")//directInput�̃o�[�W����
#pragma comment(lib,"dxguid.lib")

class Model
{
public:
	Model();
	~Model();
	//�֐�
	void initialize(XMFLOAT3 vertices[3], ID3D12Device* dev);

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList);

	void CreateBuffer(HRESULT result, ID3D12Device* dev);

private:
	//�ϐ�

	HRESULT result;

	XMFLOAT3 vertices[3];

	uint16_t indices[3];
	
	ID3D12Device* dev;

	UINT sizeVB;

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};//�q�[�v�ݒ�

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};

	//���_�o�b�t�@�̍쐬
	ID3D12Resource* vertBuff;

	XMFLOAT3* vertMap;

	//���_�o�b�t�@�[�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	
	//�萔�o�b�t�@
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//�F
	};

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};

	ID3D12Resource* constBuffMaterial;

	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial;

	//���[�g�p�����[�^
	D3D12_ROOT_PARAMETER rootParam = {};

	//�C���f�b�N�X�o�b�t�@�̐���
	ID3D12Resource* indexBuff;

	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap;

	//�C���f�b�N�X�o�b�t�@�r���[�̐���
	D3D12_INDEX_BUFFER_VIEW ibView{};

	ID3D12Resource* constBuffTransform = nullptr;

	struct ConstBufferDataTransform
	{
		XMMATRIX mat;
	};

	ConstBufferDataTransform* constMapTransform = nullptr;

};