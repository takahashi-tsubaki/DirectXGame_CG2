#include <Windows.h>

#include<tchar.h>

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	OutputDebugStringA("Hello DirectX!!\n");

	const int window_width = 960;
	const int window_height = 480;

	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DirectXTest");//�A�v���P�[�V�����N���X��(�K���ł����ł�)
	w.hInstance = GetModuleHandle(0);//�n���h���̎擾
	w.hCursor = LoadCursor(NULL, IDC_ARROW);


	RegisterClassEx(&w);//�A�v���P�[�V�����N���X(���������̍�邩���낵������OS�ɗ\������)

	RECT wrc = { 0,0,window_width, window_height };//{0,0,window_width,window_height}

	//�֐����g���ăE�B���h�E�̃T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,//�N���X���w��
		_T("DX12�e�X�g"),//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,//�\��x���W��OS�ɂ��C��
		CW_USEDEFAULT,//�\��y���W��OS�ɂ��C��
		wrc.right - wrc.left,//�E�B���h�E��
		wrc.bottom - wrc.top,//�E�B���h�E��
		nullptr,//�e�E�B���h�E�n���h��
		nullptr,//���j���[�n���h��
		w.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);//�ǉ��p�����[�^�[

	ShowWindow(hwnd, SW_SHOW);

	MSG msg{};

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (msg.message == WM_QUIT)
		{
			break;
		}
	}
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
	switch (msg) {
		//�E�B���h�E���j�����ꂽ
	case WM_DESTROY:
			//OS�ɑ΂���,�A�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}

	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}




