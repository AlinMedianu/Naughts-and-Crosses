#ifndef FX_H
#define FX_H

#include <string>
#include <d3d11.h>

//load in and allocate a chunk of binary data
char* ReadAndAllocate(const std::string& fileName, unsigned int& bytesRead);
//we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a pixel shader
void CreatePixelShader(char* pBuff, unsigned int buffSz, ID3D11PixelShader* &pPS);
//we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a vertex shader
void CreateVertexShader(char* pBuff, unsigned int buffSz, ID3D11VertexShader* &pVS);
//the input assembler needs to know what is in the vertex buffer, how to get the data out and which vertex shader to give it to
void CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, char* pBuff, unsigned int buffSz, ID3D11InputLayout** pLayout);
//if we want to give extra information to the shaders then it has to go in a constant buffer
//once a shader runs, to render a leg or something, the data can't change until it's finished
//so it's always a constant
void CreateConstantBuffer(UINT sizeOfBuffer, ID3D11Buffer **pBuffer);
//different hardware supports different instructions, lines of code, number of constants, etc
//we cheat a bit and just go for shader model 5 which is really common and powerful
void CheckShaderModel5Supported(ID3D11Device* pd3dDevice);



#endif
