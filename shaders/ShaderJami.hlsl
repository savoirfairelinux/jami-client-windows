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

sampler samplerState : register(s0);
Texture2D texY : register(t0);
Texture2D texU : register(t1);
Texture2D texV : register(t2);

float4 PS_main(VS_Output input) : SV_Target  {
    float3 yuv;
    float3 rgb;
    yuv.x = texY.Sample(samplerState,input.texCoorVSOut).r;
    yuv.y = texU.Sample(samplerState,input.texCoorVSOut).r - 0.5f;
    yuv.z = texV.Sample(samplerState,input.texCoorVSOut).r - 0.5f;

    rgb = mul(yuv, float3x3(1,1,1,
               0,-0.39465,2.03211,
               1.13983, -0.58060,  0));
    if(rgb.r < 0.1f && rgb.b < 0.1f && yuv.x < 0.1f) {
        rgb.g = 0.0f;
    }
    float4 out_px = float4(rgb,1.0f);;

    return out_px;
}