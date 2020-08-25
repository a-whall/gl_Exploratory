#shader vertex
#version 430

layout (location = 0) in vec3 VertexPosition;

out vec3 Vec;
uniform mat4 MVP;

void main() {
    Vec = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition,1.0);
}

#shader fragment
#version 430

layout(binding = 1) uniform samplerCube SkyBoxTex;
in vec3 Vec;
layout( location = 0 ) out vec4 FragColor;

void main() {
    vec3 texColor = texture(SkyBoxTex, normalize(Vec)).rgb;
    texColor = pow( texColor, vec3(1.0/2.2));
    FragColor = vec4(texColor,1);
}
#end