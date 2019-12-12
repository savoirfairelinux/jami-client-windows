uniform sampler2D Ytex;
uniform sampler2D Utex;
uniform sampler2D Vtex;

varying vec2 vTextureCoord;

varying vec3 col;

void main(void) {
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture2D(Ytex, vTextureCoord).r;
    yuv.y = texture2D(Utex, vTextureCoord).r;
    yuv.z = texture2D(Vtex, vTextureCoord).r;

    rgb = vec3(yuv.x + 1.370705 * (yuv.z - 0.5),
              yuv.x - (0.698001 * (yuv.z - 0.5)) - (0.337633 * (yuv.y - 0.5)),
              yuv.x + (1.732446 * (yuv.y - 0.5)));
    gl_FragColor = vec4(yuv, 1);
    //gl_FragColor = vec4(col,1);
    
    //gl_FragColor = texture2D(Ytex,vTextureCoord);
    //gl_FragColor = texture2D(Utex,vTextureCoord);
    //gl_FragColor = texture2D(Vtex,vTextureCoord);
}