#shader vertex
#version 450

layout (location = 0) in vec3 vin_pos;

uniform mat4 mv;

void main() {
    gl_Position = mv * vec4(vin_pos, 1.0);
}

#shader geometry
#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float sizeOver2;
uniform mat4 p;

out vec2 texCoord;

void main() {
    mat4 m = p;
    gl_Position = m * (vec4(-sizeOver2, -sizeOver2, 0.0, 0.0) + gl_in[0].gl_Position);
    texCoord = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = m * (vec4(sizeOver2, -sizeOver2, 0.0, 0.0) + gl_in[0].gl_Position);
    texCoord = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = m * (vec4(-sizeOver2, sizeOver2, 0.0, 0.0) + gl_in[0].gl_Position);
    texCoord = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = m * (vec4(sizeOver2, sizeOver2, 0.0, 0.0) + gl_in[0].gl_Position);
    texCoord = vec2(1.0, 1.0);
    EmitVertex();
}

#shader fragment
#version 450

in vec2 texCoord;

uniform sampler2D spriteTex;

layout (location = 0) out vec4 fout_color;

void main() {
    fout_color = texture(spriteTex, texCoord);
}
#end