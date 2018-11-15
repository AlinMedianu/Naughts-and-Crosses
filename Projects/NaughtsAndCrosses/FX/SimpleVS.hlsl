#include "helper.hlsl"

//given some geometry, we can transform the coordinates for display
//this is a constant buffer coming from the game
cbuffer PerObject : register(b0)
{
	float4x4 projectionMatrix;
}


VSOut SimpleVertexShader( AppData IN )
{
    VSOut OUT;

	//make sure this multiplication is the right way round!
	//matrix multiplaction is not commutative (c=A*B, but c!=B*A)
	OUT.position = mul(projectionMatrix, float4(IN.position, 1.0f));
	OUT.color = float4(IN.color, 1.0f);

    return OUT;
}

