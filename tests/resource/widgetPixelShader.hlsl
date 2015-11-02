//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------
Texture2D pd_texture0 : register(t0);
SamplerState pd_sampler0 : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Draw the entire triangle using vertex color.
    return pd_texture0.Sample(pd_sampler0, input.uv) * input.color;
}
