struct VS_Input {
    float3 VertexCoor : POSITION;
    float2 TextureCoor: Texcoordinate;
    float4 VertexColor: COLOR;
};

struct VS_Output {
    float4 positionVSOut : SV_POSITION;
    float2 texCoorVSOut  : TEXCOR_VS2PX;
    float4 vertexColor   : VERTEX_COLOR_VS2PX;
};

cbuffer VS_ConstantBuffer : register(cb0) {
    float angleToRotate;
    bool isUsingD3D11HW;
}

VS_Output VS_main(VS_Input input){
    VS_Output out_vtx;
    out_vtx.positionVSOut = float4(input.VertexCoor,1.0f);
    out_vtx.texCoorVSOut = input.TextureCoor;
    out_vtx.vertexColor = input.VertexColor;

    return out_vtx;
}

float4 PS_main(VS_Output input) : SV_Target  {
    float4 out_px = input.vertexColor;

    return out_px;
}