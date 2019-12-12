uniform sampler2D Ytex,Utex,Vtex;
varying vec2 vTextureCoord;

varying vec3 col;

void main(void) {
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture2D(Ytex, vTextureCoord).r;
    yuv.y = texture2D(Utex, vTextureCoord).r - 0.5;
    yuv.z = texture2D(Vtex, vTextureCoord).r - 0.5;
    rgb = mat3(1,1,1,
        0,-0.39465,2.03211,
        1.13983, -0.58060,  0) * yuv;
    gl_FragColor = vec4(rgb, 1);
    //gl_FragColor = vec4(col,1);
}