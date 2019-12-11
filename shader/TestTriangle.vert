uniform sampler2D Tex;
attribute vec4 posAttr;
attribute vec4 colAttr;
attribute vec2 textureCoordinate;
varying vec4 col;
varying vec2 textCor;

void main() {
    col = colAttr;
    //col = texture2D(Tex, textureCoordinate);
    textCor = textureCoordinate;
    gl_Position = posAttr;
}