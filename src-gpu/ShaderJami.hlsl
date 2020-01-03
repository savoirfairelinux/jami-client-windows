struct VS_Input {
    float3 VertexCoor : POSITION;
    float2 TextureCoor: Texcoordinate;
    float4 VertexColor: COLOR;
};

struct VS_Output {
    float4 positionVSOut : SV_POSITION;
    float2 texCoorVSOut  : TEXCOR_VS2PX;
    float4 vertexColor   : VERTEX_COLOR_VS2PX;
    bool isNV12 : USE_HW_D3D11;
};

cbuffer VS_ConstantBuffer {
    float2 aViewPortWidthAndHeight;
    float2 aWidthAndHeight;
    float angleToRotate;
    bool isNV12;
}

VS_Output VS_main(VS_Input input){
    VS_Output out_vtx;
    float3 postionAfterRotate = mul(float3x3(cos(radians(angleToRotate)),-sin(radians(angleToRotate)), 0,
                                   sin(radians(angleToRotate)),cos(radians(angleToRotate)), 0,
                                   0,                 0,                    1), input.VertexCoor);
    float2 imageTrueSizeVector;
    if((fmod(angleToRotate, 360.0) >= 45.0 && fmod(angleToRotate, 360.0) <= 135.0) ||
      (fmod(angleToRotate, 360.0) >= 225.0 && fmod(angleToRotate, 360.0) <= 315.0)) {
        imageTrueSizeVector.x = aWidthAndHeight.y;
        imageTrueSizeVector.y = aWidthAndHeight.x;
    } else {
        imageTrueSizeVector = aWidthAndHeight;
    }
    float dPictureAspectRatio = imageTrueSizeVector.x / imageTrueSizeVector.y;
    float dAspectRatio = aViewPortWidthAndHeight.x / aViewPortWidthAndHeight.y;

    if(dPictureAspectRatio > dAspectRatio) {
        float newHeight = aViewPortWidthAndHeight.x * imageTrueSizeVector.y/ 
        imageTrueSizeVector.x ;
        float heightRatio = newHeight / aViewPortWidthAndHeight.y;
        postionAfterRotate.y *= heightRatio;
    } else if(dPictureAspectRatio < dAspectRatio) {
        float newWidth = aViewPortWidthAndHeight.y * imageTrueSizeVector.x/
        imageTrueSizeVector.y;
        float widthRatio = newWidth / aViewPortWidthAndHeight.x;
        postionAfterRotate.x *= widthRatio;
    }
    
    out_vtx.positionVSOut = float4(postionAfterRotate ,1.0f);
    out_vtx.texCoorVSOut = input.TextureCoor;
    out_vtx.vertexColor = input.VertexColor;
    out_vtx.isNV12 = isNV12;

    return out_vtx;
}

static const float3x3 YUVtoRGBCoeffMatrix =
{
	1.164383f,  1.164383f, 1.164383f,
	0.000000f, -0.391762f, 2.017232f,
	1.596027f, -0.812968f, 0.000000f
};

float3 ConvertYUVtoRGB(float3 yuv)
{
	yuv -= float3(0.062745f, 0.501960f, 0.501960f);
	yuv = mul(yuv, YUVtoRGBCoeffMatrix);

	return saturate(yuv);
}

SamplerState samplerState : register(s0);
Texture2D texY : register(t0);
Texture2D texU : register(t1);
Texture2D texV : register(t2);
Texture2D texUV_NV12 : register(t3);

float4 PS_main(VS_Output input) : SV_Target  {
    
    float3 yuv;
    float3 rgb;
    if(input.isNV12) {
        float2 texCoorUV = float2(input.texCoorVSOut.x / 2, input.texCoorVSOut.y);
        
        yuv.x = texY.Sample(samplerState,input.texCoorVSOut).r;
        yuv.y = texUV_NV12.Sample(samplerState,texCoorUV).r;
        yuv.z = texUV_NV12.Sample(samplerState,texCoorUV).g;
    } else {
        yuv.x = texY.Sample(samplerState,input.texCoorVSOut).r;
        yuv.y = texU.Sample(samplerState,input.texCoorVSOut).r;
        yuv.z = texV.Sample(samplerState,input.texCoorVSOut).r;
    }
    
    rgb = ConvertYUVtoRGB(yuv);

    if(rgb.r < 0.1f && rgb.b < 0.1f && yuv.x < 0.1f) {
        rgb.g = 0.0f;
    }
    float4 out_px = float4(rgb,1.0f);;

    return out_px;
}