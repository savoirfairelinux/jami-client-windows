#ifdef GL_ES
precision mediump float;
#endif

uniform bool isNV12;
uniform sampler2D Ytex, Utex, Vtex, UVtex_NV12;
uniform vec3 vTextureCoordScalingFactors;

varying vec2 vTextureCoord;

void main(void) {
    vec3 yuv;
    vec3 rgb;

    if(isNV12) {
        vec2 texCoorUV = vec2(vTextureCoord.x / 2, vTextureCoord.y);
        
        yuv.x = texture2D(texY,vTextureCoord).r;
        yuv.y = texture2D(texUV_NV12,texCoorUV).r - 0.5;
        yuv.z = texture2D(texUV_NV12,texCoorUV).g - 0.5;
    } else {
        yuv.x = texture2D(Ytex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.x,
        vTextureCoord.y)).r;
        yuv.y = texture2D(Utex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.y,
        vTextureCoord.y)).r - 0.5;
        yuv.z = texture2D(Vtex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.z,
        vTextureCoord.y)).r - 0.5;      
    }

    rgb = mat3(1,1,1,
               0,-0.39465,2.03211,
               1.13983, -0.58060,  0) * yuv;
    
    if(rgb.r < 0.1 && rgb.b < 0.1 && yuv.x < 0.1) {
        rgb.g = 0.0;
    }
    gl_FragColor = vec4(rgb, 1);
}