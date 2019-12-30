struct VS_Input {
    float3 VertexCoor : POSITION_Vertex;
    float2 TextureCoor: Texcoordinate;
    float4 VertexColor: ColorVertex;
};

struct VS_Output {
    float3 positionVSOut : SV_POSITION;
    float3 texCoorVSOut  : TEXCOR_VS2PX;
    float4 vertexColor   : VERTEX_COLOR_VS2PX;
};

VS_Output VS_main(VS_Input input){
    VS_Output out_vtx;

    return out_vtx;
}

struct PS_Output {
    float4 ColorOutput : SV_TARGET;
};

PS_Output PS_main(VS_Output input) {
    PS_Output out_px;

    return out_px;
}