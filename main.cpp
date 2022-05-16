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

//リンクの設定
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")//directInputのバージョン
#pragma comment(lib,"dxguid.lib")

//関数のプロトタイプ宣言
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#pragma region 入力関数
//キーの初期化処理関数
void keyInitialize(BYTE* key,BYTE* oldkey, int array);
//キーが押されてる時
bool pushKey(BYTE key,int keyNum);
//キーが押されてない時
bool notPushKey(BYTE* key, int keyNum);
//キーが押した時
bool pressKey(BYTE* key, BYTE* oldkey, int keyNum);
//キー長押ししてる時
bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum);
//キーを離した時
bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum);
#pragma endregion 入力関数


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	OutputDebugStringA("Hello DirectX!!\n");

	const int window_width = 960;
	const int window_height = 640;

	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;//コールバック関数の指定
	w.lpszClassName = _T("DirectXTest");//アプリケーションクラス名(適当でいいです)
	w.hInstance = GetModuleHandle(0);//ハンドルの取得
	w.hCursor = LoadCursor(NULL, IDC_ARROW);


	RegisterClassEx(&w);//アプリケーションクラス(こういうの作るからよろしくってOSに予告する)

	RECT wrc = { 0,0,window_width, window_height };//{0,0,window_width,window_height}

	//関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(w.lpszClassName,//クラス名指定
		_T("DX12Test"),//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,//表示x座標はOSにお任せ
		CW_USEDEFAULT,//表示y座標はOSにお任せ
		wrc.right - wrc.left,//ウィンドウ幅
		wrc.bottom - wrc.top,//ウィンドウ高
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		w.hInstance,//呼び出しアプリケーションハンドル
		nullptr);//追加パラメーター

	ShowWindow(hwnd, SW_SHOW);

	MSG msg{};

	//ここからDirectX初期化処理
#ifdef _DEBUG
	//デバックレイヤーをオンに
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	HRESULT result;
	//受け皿となる変数
	ID3D12Device* dev = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGISwapChain4* swapChain = nullptr;
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeap = nullptr;


	//DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	//アダプターの列挙用
	std::vector<IDXGIAdapter4*> adapters;

	//ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter4* tmpAdapter = nullptr;

	//パフォーマンスの高い順から、全てのアダプターを列挙する
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&tmpAdapter))
		!= DXGI_ERROR_NOT_FOUND; i++)
	{
		//動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	//妥当なアダプターを選別する
	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[i];
			break;
		}
	}


	//対応レベルの取得
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;
	for (size_t i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//デバイスを生成した時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	//コマンドアロケータを生成
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
	assert(SUCCEEDED(result));

	//コマンドリストを生成
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(result));

	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//コマンドキューを生成
	result = dev->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));

	//スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = window_width;
	swapChainDesc.Height = window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色情報の書式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//バックバッファ用
	swapChainDesc.BufferCount = 2;//バッファ数を2つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//フリップ後は破棄
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//
	//スワップチェーンの生成
	result = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapChain);
	assert(SUCCEEDED(result));

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//レンダーターゲットビュー
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;//裏表の2つ

	//デスクリプタヒープの生成
	dev->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));//先にデスクリプタヒープを作る

	//バックバッファ
	std::vector<ID3D12Resource*>backBuffers;
	backBuffers.resize(swapChainDesc.BufferCount);//スワップチェーン内に生成されたバックバッファのアドレスを入れておくためのポインタを用意する

	//スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers.size(); i++)
	{
		//スワップチェーンからバッファを取得
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//表か裏かでアドレスがずれる
		rtvHandle.ptr += i * dev->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダーターゲットビューの生成
		dev->CreateRenderTargetView(backBuffers[i], &rtvDesc, rtvHandle);
	}

	//フェンスの生成
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//DirectInputの初期化
	IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
	//DISCL_FOREGROUND   画面が手前にある場合のみ入力を受け付ける
	//DISCL_NONEXCLUSIVE デバイスをこのアプリだけで専有しない
	//DISCL_NOWINKEY     Windowsキーを無効にする



	//ここまでDirectX初期化処理


	//ここから描画初期化処理
	//頂点データ
	XMFLOAT3 vertices[] =
	{
		// x     y    z    //座標
		{-0.5f,+0.5f,0.0f},//左上
		{+0.5f,+0.5f,0.0f},//右上
		{-0.5f,-0.5f,0.0f},//左下
		{+0.5f,-0.5f,0.0f},//右下
	};

	uint16_t indices[] =
	{
		0,1,2,
		1,2,3,
	};
	
	

	//頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点の要素数
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの作成
	ID3D12Resource* vertBuff = nullptr;
	result = dev->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];//座標をコピー
	}

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファービューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	ID3DBlob* vsBlob = nullptr;//頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr;//ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr;//エラーオブジェクト

	//定数バッファ
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;//色
	};

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
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

	ID3D12Resource* constBuffMaterial = nullptr;
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
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(1,0,0,0.5f);

	//ルートパラメータ
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParam.Descriptor.ShaderRegister = 0;//定数バッファ番号
	rootParam.Descriptor.RegisterSpace = 0;//デフォルト値
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える

	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t)) * _countof(indices);

	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	ID3D12Resource* indexBuff = nullptr;
	result = dev->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	
	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];
	}
	//マッピング解除
	indexBuff->Unmap(0,nullptr);

	//インデックスバッファビューの生成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format =DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//頂点シェーダの読み込みとコンパイル(頂点シェーダは頂点の座標変換)
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルードを可能にする
		"main",//エントリーポイント
		"vs_5_0",//シェーダモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバック用設定
		0,
		&vsBlob, &errorBlob);

	////コンパイルエラーなら
	//if (FAILED(result))
	//{
	//	//errorBlobからエラー内容をstirng型にコピー
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferPointer(),
	//		error.begin());
	//	error += "\n";
	//	//エラー内容を出力ウィンドウに表示
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}

	//ピクセルシェーダの読み込みとコンパイル(ピクセルの役割は描画色の設定)
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);

	////コンパイルエラーなら
	//if (FAILED(result))
	//{
	//	//errorBlobからエラー内容をstirng型にコピー
	//	std::string error;
	//	error.resize(errorBlob->GetBufferSize());

	//	std::copy_n((char*)errorBlob->GetBufferPointer(),
	//		errorBlob->GetBufferPointer(),
	//		error.begin());
	//	error += "\n";
	//	//エラー内容を出力ウィンドウに表示
	//	OutputDebugStringA(error.c_str());
	//	assert(0);
	//}


	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},
	};

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

	//ラスタライザの設定(頂点のピクセル化)
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に

	//ブレンドステート
	/*pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;*///RGBA全てのチャンネルを描画
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//共通設定
	//blenddesc.BlendEnable    = true;//ブレンドを有効にする
	//blenddesc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlendAlpha  = D3D12_BLEND_ONE;//ソースの値を100%使う
	//blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//ソースの値を0%使う

	////	加算合成
	//blenddesc.BlendOp   = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlend  = D3D12_BLEND_ONE;//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;//ソースの値を100%使う

	////減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//テストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;//ソースの値を100%使う

	////色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;//使わない

	//半透明
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	//blenddesc.DestBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-ソースのアルファ値

	//頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	pipelineDesc.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;//1ピクセルにつき1回レンダリング

	//ルートシグネチャ
	ID3D12RootSignature* rootSignature;

	//ルートシグネチャの設定(生成)
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = &rootParam;
	rootSignatureDesc.NumParameters = 1;

	//ルートシグネチャのシリアライズ
	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	rootSigBlob->Release();

	//パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = rootSignature;

	//パイプラインステートの生成
	ID3D12PipelineState* pipelineState = nullptr;
	result = dev->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	//ここまで描画初期化処理

	FLOAT clearColor[] = { 0.1f, 0.25f, 0.5f, 0.0f };//青っぽい色

	BYTE key[256] = {};
	BYTE oldkey[256] = {};

	//ゲームループ
	while (true)
	{
		//ウィンドウメッセージ処理
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

		//ここからDirectX毎フレーム処理

		//キーボード情報の取得開始
		keyboard->Acquire();
		keyInitialize(key, oldkey, sizeof(key) / sizeof(key[0]));
		//
		keyboard->GetDeviceState(sizeof(key), key);

		int angle = 0;
		if (pressKey(key, oldkey, DIK_Z))
		{
			angle = 180;
		}
		else if (pressKey(key, oldkey, DIK_C))
		{
			angle = -180;
		}
		float cosin = cos(PI / angle);
		float sain = sin(PI / angle);
		
		float transX = 0.0f;
		if (pressKey(key, oldkey, DIK_D))
		{
			transX = 0.01f;
		}
		else if (pressKey(key, oldkey, DIK_A))
		{
			transX = -0.01f;
		}
		float transY = 0.0f;
		if (pressKey(key, oldkey, DIK_W))
		{
			transY = 0.01f;
		}
		else if (pressKey(key, oldkey, DIK_S))
		{
			transY = -0.01f;
		}
		float transZ = 0.0f;

		float afinRotaZ[4][4] =
		{
		  {cosin,sain,0.0f,0.0f},//x=cosΘ-ysinΘ
		  {-sain,cosin,0.0f,0.0f},//y=xsinΘ+ycosΘ
		  {0.0f,0.0f,1.0f,0.0f},//z=z
		  {0.0f,0.0f,0.0f,1.0f},//
		};

		float afinScale[4][4] =
		{
			{1.01f,0.0f,0.0f,0.0f},//x軸
			{0.0f,1.01f,0.0f,0.0f},//y軸
			{0.0f,0.0f,1.01f,0.0f},//z軸
			{0.0f,0.0f,0.0f,1.0f},//？
		};

		float afinShrink[4][4] =
		{
			{0.99f,0.0f,0.0f,0.0f},//x軸
			{0.0f,0.99f,0.0f,0.0f},//y軸
			{0.0f,0.0f,0.99f,0.0f},//z軸
			{0.0f,0.0f,0.0f,1.0f},//？
		};

		float afinTranslation[4][4] =
		{
		  {1.0f, 0.0f, 0.0f, transX},//Tx
		  {0.0f, 1.0f, 0.0f, transY},//Ty
		  {0.0f, 0.0f, 1.0f, transZ},//Tz
		  {0.0f, 0.0f, 0.0f, 1.0f},//1
		};

		if (pressKey(key,oldkey,DIK_Z)|| pressKey(key, oldkey, DIK_C))
		{
			 
			 for (int i = 0; i < 4/* _countof(vertices)*/; i++)
			 {
				 vertices[i].x = vertices[i].x * afinRotaZ[0][0] + vertices[i].y * afinRotaZ[0][1] +
					 vertices[i].z * afinRotaZ[0][2] + 1 * afinRotaZ[0][3];
				 vertices[i].y = vertices[i].x * afinRotaZ[1][0] + vertices[i].y * afinRotaZ[1][1] +
					 vertices[i].z * afinRotaZ[1][2] + 1 * afinRotaZ[1][3];
				 vertices[i].z = vertices[i].x * afinRotaZ[2][0] + vertices[i].y * afinRotaZ[2][1] +
					 vertices[i].z * afinRotaZ[2][2] + 1 * afinRotaZ[2][3];
			 }
		}
		if (pressKey(key, oldkey, DIK_2))
		{
			for (int i = 0; i < 4/* _countof(vertices)*/; i++)
			{
				vertices[i].x = vertices[i].x * afinScale[0][0] + vertices[i].y * afinScale[0][1] +
					vertices[i].z * afinScale[0][2] + 1 * afinScale[0][3];
				vertices[i].y = vertices[i].x * afinScale[1][0] + vertices[i].y * afinScale[1][1] +
					vertices[i].z * afinScale[1][2] + 1 * afinScale[1][3];
				vertices[i].z = vertices[i].x * afinScale[2][0] + vertices[i].y * afinScale[2][1] +
					vertices[i].z * afinScale[2][2] + 1 * afinScale[2][3];
			}
			
		}
		if (pressKey(key, oldkey, DIK_3))
		{
			for (int i = 0; i < 4/* _countof(vertices)*/; i++)
			{
				vertices[i].x = vertices[i].x * afinShrink[0][0] + vertices[i].y * afinShrink[0][1] +
					vertices[i].z * afinShrink[0][2] + 1 * afinShrink[0][3];
				vertices[i].y = vertices[i].x * afinShrink[1][0] + vertices[i].y * afinShrink[1][1] +
					vertices[i].z * afinShrink[1][2] + 1 * afinShrink[1][3];
				vertices[i].z = vertices[i].x * afinShrink[2][0] + vertices[i].y * afinShrink[2][1] +
					vertices[i].z * afinShrink[2][2] + 1 * afinShrink[2][3];
			}

		}
		if (pressKey(key, oldkey, DIK_W)|| pressKey(key, oldkey, DIK_S)
			|| pressKey(key, oldkey, DIK_A)|| pressKey(key, oldkey, DIK_D))
		{
			for (int i = 0; i < 4/* _countof(vertices)*/; i++)
			{
				vertices[i].x = vertices[i].x * afinTranslation[0][0] + vertices[i].y * afinTranslation[0][1] +
					vertices[i].z * afinTranslation[0][2] + 1 * afinTranslation[0][3];
				vertices[i].y = vertices[i].x * afinTranslation[1][0] + vertices[i].y * afinTranslation[1][1] +
					vertices[i].z * afinTranslation[1][2] + 1 * afinTranslation[1][3];
				vertices[i].z = vertices[i].x * afinTranslation[2][0] + vertices[i].y * afinTranslation[2][1] +
					vertices[i].z * afinTranslation[2][2] + 1 * afinTranslation[2][3];
			}

		}

		for (int i = 0; i < _countof(vertices); i++)
		{
			vertMap[i] = vertices[i];//座標をコピー
		}


		//バックバッファの番号を解除
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

		// １．リソースバリアで書き込み可能に変更
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex];//バックバッファを指定
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//表示状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;//描画状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		//　２．描画先の変更
		//レンダーターゲットビューのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		// ３．画面クリア          R     G     B     A(alpha)

		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		//　４．ここから描画コマンド
		//ビューポートの設定コマンド
		D3D12_VIEWPORT viewport[4]{};

		viewport[0].Width = window_width;
		viewport[0].Height = window_height;
		viewport[0].TopLeftX = 0;
		viewport[0].TopLeftY = 0;
		viewport[0].MinDepth = 0.0f;
		viewport[0].MaxDepth = 1.0f;

		/*viewport[1].Width = window_width;
		viewport[1].Height = window_height;
		viewport[1].TopLeftX = -300;
		viewport[1].TopLeftY = 200;
		viewport[1].MinDepth = 0.0f;
		viewport[1].MaxDepth = 1.0f;

		viewport[2].Width = window_width;
		viewport[2].Height = window_height;
		viewport[2].TopLeftX = 200;
		viewport[2].TopLeftY = -100;
		viewport[2].MinDepth = 0.0f;
		viewport[2].MaxDepth = 1.0f;

		viewport[3].Width = window_width;
		viewport[3].Height = window_height / 2;
		viewport[3].TopLeftX = 200;
		viewport[3].TopLeftY = 400;
		viewport[3].MinDepth = 0.0f;
		viewport[3].MaxDepth = 1.0f;*/


		//ビューポート設定コマンドを、コマンドリストに積む
		commandList->RSSetViewports(1, &viewport[0]);

		//シザー矩形
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;//切り抜き座標左
		scissorRect.right = scissorRect.left + window_width;//切り抜き座標右
		scissorRect.top = 0;//切り抜き座標上
		scissorRect.bottom = scissorRect.top + window_height;//切り抜き座標下

		//シザー矩形設定コマンドを、コマンドリストに積む
		commandList->RSSetScissorRects(1, &scissorRect);

		//パイプラインステートとルートシグネチャの設定コマンド
		
		commandList->SetPipelineState(pipelineState);

		
		commandList->SetGraphicsRootSignature(rootSignature);

		//プリミティブ形状の設定コマンド
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		/*commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);*/
		
		//頂点バッファービューの設定コマンド
		commandList->IASetVertexBuffers(0, 1, &vbView);

		//定数バッファビュー(CBV)の設定コマンド
		commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		//インデックスバッファビューの設定コマンド
		commandList->IASetIndexBuffer(&ibView);

		//描画コマンド
		
		commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0,0);//全ての頂点を使って描画
		//ビューポート設定コマンドを、コマンドリストに積む
		//commandList->RSSetViewports(1, &viewport[1]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//全ての頂点を使って描画

		//commandList->RSSetViewports(1, &viewport[2]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//全ての頂点を使って描画

		//commandList->RSSetViewports(1, &viewport[3]);
		//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);//全ての頂点を使って描画
		//　４．ここまで描画コマンド

		//　５．リソースバリアを戻す
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//描画状態から
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//表示状態へ
		commandList->ResourceBarrier(1, &barrierDesc);

		//命令のクローズ
		result = commandList->Close();
		assert(SUCCEEDED(result));

		//コマンドリストの実行
		ID3D12CommandList* commandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(1, commandLists);

		//画面に表示するバッファをフリップ (裏表の入れ替え)
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));

		//コマンドの実行完了を待つ
		commandQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal)
		{
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		//キューをクリア
		result = cmdAllocator->Reset();
		assert(SUCCEEDED(result));
		//再びコマンドリストをためる準備
		result = commandList->Reset(cmdAllocator, nullptr);
		assert(SUCCEEDED(result));

		//ここまでDirectX毎フレーム処理
	}
	UnregisterClass(w.lpszClassName, w.hInstance);


	return 0;
}

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して,アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void keyInitialize(BYTE* key, BYTE* oldkey, int array)
{
	for (int i = 0; i < array; i++)
	{
		oldkey[i] = key[i];
	}
};
bool pushKey(BYTE* key, int keyNum)
{
	if (key[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool notPushKey(BYTE* key, int keyNum)
{
	if (!key[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}

};

bool pressKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (key[keyNum] && oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool triggerKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (key[keyNum] && !oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

bool releaseKey(BYTE* key, BYTE* oldkey, int keyNum)
{
	if (!key[keyNum] && oldkey[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
};

