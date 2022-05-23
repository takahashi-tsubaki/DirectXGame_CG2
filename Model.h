#pragma once
#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <wrl.h>

using namespace DirectX;

const float PI = 3.141592f;

//�����N�̐ݒ�
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")//directInput�̃o�[�W����
#pragma comment(lib,"dxguid.lib")

class Model
{

public:
	//�ϐ�
	XMFLOAT3 vertices[3] =
	{
		// x     y    z    //���W
		{-0.5f,+0.5f,0.0f},//����
		{-0.5f,-0.5f,0.0f},//����
		{+0.5f,-0.5f,0.0f},//�E��
	};

	uint16_t indices[3] =
	{
		0,1,2,
	};
	
	ID3D12Device* dev = nullptr;
public:
	Model(XMFLOAT3 vertices[3], uint16_t indices[3], ID3D12Device* dev);
	~Model();
	//�֐�
	void initialize(XMFLOAT3 vertices[3], ID3D12Device* dev);

	void Update();
};