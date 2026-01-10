
#version 330 core

out vec4 outColor;
in vec4 fragColor;

void main(){
    vec3 mul = gl_FragCoord.xyz/1920;
    outColor = vec4(mul, fragColor.a);
}