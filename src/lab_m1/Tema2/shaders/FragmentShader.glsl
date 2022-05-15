#version 330

// Input
in vec3 frag_color;
in vec3 frag_normal;
in vec2 frag_texture;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec3 out_texture;

void main()
{
    out_color = vec4(frag_color, 1);
    out_normal = vec4(frag_normal, 1);
    out_texture = vec3(frag_texture, 1);
}
