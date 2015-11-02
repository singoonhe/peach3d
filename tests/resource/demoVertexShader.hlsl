// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------
cbuffer GlobalConstantBuffer : register(b0)
{
    matrix pd_projMatrix;
    matrix pd_viewMatrix;
    float4 pd_ambient;
};
cbuffer ObjectConstantBuffer : register(b1)
{
    matrix pd_modelMatrix;
};

struct VertexShaderInput
{
    float3 pos : pd_vertex;
    float3 color : pd_color;
    float2 uv : pd_uv;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;
    float4 pos = float4(input.pos, 1.0f);

    // convert position, DX not support * ? 
    pos = mul(pos, pd_modelMatrix);
    pos = mul(pos, pd_viewMatrix);
    vertexShaderOutput.pos = mul(pos, pd_projMatrix);
    // Reture value direct
    vertexShaderOutput.color = input.color * pd_ambient.rgb;
    vertexShaderOutput.uv = input.uv;

    return vertexShaderOutput;
}
