uniform sampler2D Ytex;
uniform sampler2D Utex;
uniform sampler2D Vtex;

varying vec2 vTextureCoord;

varying vec3 col;

void main(void) {
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture2D(Ytex, vTextureCoord).r;
    yuv.y = texture2D(Utex, vTextureCoord).g;
    yuv.z = texture2D(Vtex, vTextureCoord).b;

    rgb = mat3(1,1,1,
        0,-0.39465,2.03211,
        1.13983, -0.58060,  0) * yuv;
    //gl_FragColor = vec4(yuv, 1);
    //gl_FragColor = vec4(col,1);
    
    //gl_FragColor = texture2D(Ytex,vTextureCoord);
    //gl_FragColor = texture2D(Utex,vTextureCoord);
    gl_FragColor = texture2D(Vtex,vTextureCoord);
}