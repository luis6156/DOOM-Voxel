#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;
layout(location = 3) in vec2 v_texture;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 Color;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_color;
out vec2 frag_texture;

void main()
{
    frag_position = v_position;
    frag_normal = v_normal;
    frag_color = Color; // Original version
    frag_texture = v_texture;

    gl_Position = Projection * View * Model * vec4(v_position, 1);
}
