struct VS_Input {
    float3 VertexCoor : POSITION_Vertex;
    float3 TextureCoor: Texcoordinate;
    float4 VertexColor: ColorVertex;
};

struct VS_Output {
    float3 positionOut : SV_POSITION;
    
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