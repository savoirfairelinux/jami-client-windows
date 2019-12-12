#ifdef GL_ES
precision mediump float;
#endif

uniform float aAngleToRotate;
uniform vec2 aViewPortWidthAndHeight;
uniform vec2 aWidthAndHeight;
attribute vec3 aPosition;
attribute vec2 aTextureCoord;
varying vec2 vTextureCoord;

void main() {
    vec3 postionAfterRotate = mat3(cos(radians(aAngleToRotate)),sin(radians(aAngleToRotate)), 0,
                                   -sin(radians(aAngleToRotate)),cos(radians(aAngleToRotate)), 0,
                                   0,                 0,                    1) * aPosition;
    vec2 imageTrueAspectRatio = aWidthAndHeight;
    if(mod(aAngleToRotate, 360) >= 45 && mod(aAngleToRotate, 360) <= 135) {
        imageTrueAspectRatio.x = aWidthAndHeight.y;
        imageTrueAspectRatio.y = aWidthAndHeight.x;
    }
    float ratio = 1.0;
    float screenRatio = 1.0;
    mat3 scaleMat;
    if(aWidthAndHeight.x >= aWidthAndHeight.y) {
        ratio = aWidthAndHeight.y / aWidthAndHeight.x;
    } else {
        ratio = aWidthAndHeight.x / aWidthAndHeight.y;     
    }
    if(aViewPortWidthAndHeight.x >= aViewPortWidthAndHeight.y) {
        screenRatio = aViewPortWidthAndHeight.y / aViewPortWidthAndHeight.x;
    } else {
        screenRatio = aViewPortWidthAndHeight.x / aViewPortWidthAndHeight.y;     
    }

    if(aViewPortWidthAndHeight.x / imageTrueAspectRatio.x >
       aViewPortWidthAndHeight.y / imageTrueAspectRatio.y) {
        scaleMat = mat3(ratio * screenRatio, 0, 0,
                        0, 1, 0,
                        0, 0, 1);
    } else if(aViewPortWidthAndHeight.x / imageTrueAspectRatio.x <
       aViewPortWidthAndHeight.y / imageTrueAspectRatio.y){
        scaleMat = mat3(1, 0, 0,
                        0, ratio * screenRatio, 0,
                        0, 0, 1);
    } else {
        scaleMat = mat3(1, 0, 0,
                        0, 1, 0,
                        0, 0, 1);
    }
    postionAfterRotate = scaleMat * postionAfterRotate;
    gl_Position = vec4(postionAfterRotate,1);
    vTextureCoord = aTextureCoord;
}