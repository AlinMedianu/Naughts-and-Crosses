#include "helper.hlsl"

float4 SimplePixelShader(VSOut IN) : SV_TARGET
{
	//tell the gpu which colour to use for a screen pixel
	return IN.color;
}
