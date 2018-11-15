//data coming from the game
struct AppData
{
	float3 position : POSITION;
	float3 color: COLOR;
};
//data going out of the shader
struct VSOut
{
	float4 color : COLOR;
	float4 position : SV_POSITION;
};

