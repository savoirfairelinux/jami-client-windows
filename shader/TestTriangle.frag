uniform sampler2D Tex;
varying vec4 col;
varying vec2 textCor;

void main() {
    //gl_FragColor = col;
    gl_FragColor = texture2D(Tex, textCor);
    //gl_FragColor = vec4(0.0471, 0.0157, 0.0157, 1.0);
}