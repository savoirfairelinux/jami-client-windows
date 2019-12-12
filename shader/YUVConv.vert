uniform float aAngleToRotate;
attribute vec3 aPosition;
attribute vec2 aTextureCoord;
varying vec2 vTextureCoord;

void main() {
    vec3 postionAfterRotate = mat3(cos(aAngleToRotate),sin(aAngleToRotate), 0,
                                   -sin(aAngleToRotate),cos(aAngleToRotate), 0,
                                   0,                 0,                    1)
                                * aPosition;
    gl_Position = vec4(postionAfterRotate,1);
    vTextureCoord = aTextureCoord;
}