//ピクセルシェーダ
cbuffer ConstBufferDateMaterial : register(b0)
{
	float4 color;
}
float4 main() : SV_TARGET
{
	//色設定
	/*return float4(1.0f, 1.0f, 1.0f, 1.0f);*/
	return color;
}