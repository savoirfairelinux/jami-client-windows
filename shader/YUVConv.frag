#ifdef GL_ES
precision mediump float;
#endif

uniform bool isNV12;
uniform sampler2D Ytex, Utex, Vtex, UVtex_NV12;
uniform vec3 vTextureCoordScalingFactors;

varying vec2 vTextureCoord;

mat3 YUVtoRGBCoeffMatrix = mat3(1.164383,  1.164383, 1.164383,
	                            0.000000, -0.391762, 2.017232,
	                            1.596027, -0.812968, 0.000000);

vec3 ConvertYUVtoRGB(vec3 yuv)
{
	yuv -= vec3(0.062745, 0.501960, 0.501960);
	yuv = mul(yuv,transpose(YUVtoRGBCoeffMatrix));

	return saturate(yuv);
}

void main(void) {
    vec3 yuv;
    vec3 rgb;

    if(isNV12) {
        vec2 texCoorUV = vec2(vTextureCoord.x / 2, vTextureCoord.y);
        
        yuv.x = texture2D(Ytex, vTextureCoord).r;
        yuv.y = texture2D(UVtex_NV12,texCoorUV).r;
        yuv.z = texture2D(UVtex_NV12,texCoorUV).g;
    } else {
        yuv.x = texture2D(Ytex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.x,
        vTextureCoord.y)).r;
        yuv.y = texture2D(Utex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.y,
        vTextureCoord.y)).r;
        yuv.z = texture2D(Vtex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.z,
        vTextureCoord.y)).r;      
    }

    rgb = ConvertYUVtoRGB(yuv);

    if(rgb.r < 0.1 && rgb.b < 0.1 && yuv.x < 0.1) {
        rgb.g = 0.0;
    }
    gl_FragColor = vec4(rgb, 1);
}