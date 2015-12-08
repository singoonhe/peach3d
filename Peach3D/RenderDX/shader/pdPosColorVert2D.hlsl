
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float2 pos : pd_vertex;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    output.pos = float4(input.pos, 0.0f, 1.0f);

    return output;
}
