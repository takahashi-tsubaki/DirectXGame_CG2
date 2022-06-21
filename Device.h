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

//リンクの設定
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")//directInputのバージョン
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

	//キーの初期化処理関数
	void keyInitialize(BYTE* key, BYTE* oldkey, int array);
	//キーが押されてる時
	bool pushKey(BYTE key, int keyNum);
	//キーが押されてない時
	bool notPushKey(BYTE* key, int keyNum);
	//キーが押した時
	bool pressKey(BYTE* key, BYTE* oldkey, int keyNum);
	//キー長押ししてる時
	bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum);
	//キーを離した時
	bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum);
};