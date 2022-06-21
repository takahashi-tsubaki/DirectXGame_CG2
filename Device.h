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

//�����N�̐ݒ�
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")//directInput�̃o�[�W����
#pragma comment(lib,"dxguid.lib")
class Device
{
public:

	HRESULT result;

	BYTE key[256] = {};
	BYTE oldkey[256] = {};

	IDirectInputDevice8* keyboard;

public:
	void Initalize();
	void Update();

	//�L�[�̏����������֐�
	void keyInitialize(BYTE* key, BYTE* oldkey, int array);
	//�L�[��������Ă鎞
	bool pushKey(BYTE key, int keyNum);
	//�L�[��������ĂȂ���
	bool notPushKey(BYTE* key, int keyNum);
	//�L�[����������
	bool pressKey(BYTE* key, BYTE* oldkey, int keyNum);
	//�L�[���������Ă鎞
	bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum);
	//�L�[�𗣂�����
	bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum);
};