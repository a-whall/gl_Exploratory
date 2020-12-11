#shader vertex
#version 450

layout (location = 0) in vec3 vin_pos;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vin_pos, 1.0);
}

#shader fragment
#version 450

layout (location = 0) out vec4 fout_color;

void main() {
    fout_color = vec4(.5, .9, .9, 0.4);
}
#end