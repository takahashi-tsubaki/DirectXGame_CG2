#include "Device.h"
//void Device::Initalize()
//{
//	//キーボードデバイスの生成
//	IDirectInputDevice8* keyboard = nullptr;
//	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
//	assert(SUCCEEDED(result));
//
//	//入力データ形式のセット
//	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
//	assert(SUCCEEDED(result));
//
//	//排他制御レベルのセット
//	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
//	assert(SUCCEEDED(result));
//
//}
//
//void Device::Update()
//{
//	keyboard->Acquire();
//	keyInitialize(key, oldkey, sizeof(key) / sizeof(key[0]));
//	//
//	keyboard->GetDeviceState(sizeof(key), key);
//}
//
//void keyInitialize(BYTE* key, BYTE* oldkey, int array)
//{
//	for (int i = 0; i < array; i++)
//	{
//		oldkey[i] = key[i];
//	}
//};
//bool pushKey(BYTE* key, int keyNum)
//{
//	if (key[keyNum])
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//};
//
//bool notPushKey(BYTE* key, int keyNum)
//{
//	if (!key[keyNum])
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//
//};
//
//bool pressKey(BYTE* key, BYTE* oldkey, int keyNum)
//{
//	if (key[keyNum] && oldkey[keyNum])
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//};
//
//bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum)
//{
//	if (key[keyNum] && !oldkey[keyNum])
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//};
//
//bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum)
//{
//	if (!key[keyNum] && oldkey[keyNum])
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//};
