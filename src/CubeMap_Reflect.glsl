#shader vertex
#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 ReflectDir;

uniform vec3 WorldCameraPosition;

uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    vec3 worldPos = vec3( ModelMatrix * vec4(VertexPosition,1.0) );
    vec3 worldNorm = vec3(ModelMatrix * vec4(VertexNormal, 0.0));
    vec3 worldView = normalize( WorldCameraPosition - worldPos );

    ReflectDir = reflect(-worldView, worldNorm );
    
    gl_Position = MVP * vec4(VertexPosition,1.0);
}

#shader fragment
#version 430

in vec3 ReflectDir;

layout(binding=0) uniform samplerCube CubeMapTex;

uniform float ReflectFactor;
uniform vec4 MaterialColor;

layout( location = 0 ) out vec4 FragColor;

void main() {
    // Access the cube map texture
    vec3 cubeMapColor = texture(CubeMapTex, ReflectDir).rgb;
    // Gamma correct
    cubeMapColor = pow(cubeMapColor, vec3(1.0/2.2));
    FragColor = vec4( mix( MaterialColor.rgb, cubeMapColor, ReflectFactor), 1);
}