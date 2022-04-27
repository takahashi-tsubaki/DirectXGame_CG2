#include "Basic.hlsli"

float4 main(VSOutput input) : SV_TARGET
{
	return float4(input.uv,0,1);
}

//float4 main() : SV_TARGET
//{
//	//Fİ’è
//	/*return float4(1.0f, 1.0f, 1.0f, 1.0f);*/
//	return color;
//}