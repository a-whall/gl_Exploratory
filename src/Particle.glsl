#shader vertex
#version 430

layout (location = 0) in vec4 VertexPosition;

out vec4 Position;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
    Position = ModelViewMatrix * VertexPosition;
    gl_Position = MVP * Position;
}

#shader fragment
#version 430

in vec4 Position;

uniform vec4 Color;

layout( location = 0 ) out vec4 FragColor;

void main() {
  FragColor = Color;
}
#end