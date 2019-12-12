attribute vec4 aPosition;
attribute vec2 aTextureCoord;
varying vec2 vTextureCoord;

attribute vec3 aColor;
varying vec3 col;

void main() {
    gl_Position = aPosition;
    vTextureCoord = aTextureCoord;
    col = aColor;
}