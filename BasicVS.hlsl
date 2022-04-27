#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;//ピクセルシェーダに渡す値
	output.svpos = pos;
	output.uv = uv;
	return output;
}

////頂点シェーダ
//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos * float4(0.5f,0.5f,1,1);
//}