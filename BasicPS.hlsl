//�s�N�Z���V�F�[�_
cbuffer ConstBufferDateMaterial : register(b0)
{
	float4 color;
}
float4 main() : SV_TARGET
{
	//�F�ݒ�
	/*return float4(1.0f, 1.0f, 1.0f, 1.0f);*/
	return color;
}