#ifndef SHADERTYPES_H
#define SHADERTYPES_H

#include <d3d11.h>

#include "SimpleMath.h"

/*
This is what our vertex data will look like
We can call it whatever we like
*/
struct VertexPosColour
{
	//xyz coordinate of some geometry
	DirectX::SimpleMath::Vector3 Pos;
	//colour is Red Green Blue Alpha (the alpha is related to transparency, ignore it for now)
	DirectX::SimpleMath::Vector4 Colour;

	//this structure needs to presented/explained to D3D so it knows what to expect
	//and can process it properly. This description is useful for that purpose.
	//static=only ever one of these
	//const=it doesn't change
	static const D3D11_INPUT_ELEMENT_DESC sVertexDesc[2];
};

/*
This is a constant buffer providing important information
to the shader. It's size must be a multiple of 16bytes (16byte aligned).
*/
struct GfxParamsPerObj
{
	//a local space coordiante needs transforming to world space
	//then view space, then projecting into screen coordinates
	//this matrix does all of that in one
	DirectX::SimpleMath::Matrix wvp;
};


static_assert((sizeof(GfxParamsPerObj) % 16) == 0, "CB size not padded correctly");


#endif
