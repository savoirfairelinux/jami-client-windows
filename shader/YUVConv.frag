#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D Ytex, Utex, Vtex;
uniform vec3 vTextureCoordScalingFactors;

varying vec2 vTextureCoord;

void main(void) {
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture2D(Ytex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.x,
    vTextureCoord.y)).r;
    yuv.y = texture2D(Utex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.y,
    vTextureCoord.y)).r - 0.5;
    yuv.z = texture2D(Vtex, vec2(vTextureCoord.x * vTextureCoordScalingFactors.z,
    vTextureCoord.y)).r - 0.5;

    rgb = mat3(1,1,1,
               0,-0.39465,2.03211,
               1.13983, -0.58060,  0) * yuv;
    gl_FragColor = vec4(rgb, 1);
}