#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;//�s�N�Z���V�F�[�_�ɓn���l
	output.svpos = pos;
	output.uv = uv;
	return output;
}

////���_�V�F�[�_
//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos * float4(0.5f,0.5f,1,1);
//}