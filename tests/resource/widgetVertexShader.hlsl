// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------

struct VertexShaderInput
{
    float2 pos : pd_vertex;
    float2 uv : pd_uv;
    float4 color : pd_color;
    matrix modelMat : pd_modelMatrix;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;
    float4 pos = float4(input.pos, 0.0f, 1.0f);

    // convert position, DX not support * ? 
    vertexShaderOutput.pos = mul(pos, input.modelMat);
    // Reture value direct
    vertexShaderOutput.uv = input.uv;
    vertexShaderOutput.color = input.color;

    return vertexShaderOutput;
}
