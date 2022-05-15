#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;
layout(location = 3) in vec2 v_texture;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 Color;
uniform int Health;
uniform bool isDying;
uniform float Time;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_color;
out vec2 frag_texture;

float rand3D(in vec3 co){
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233,144.7272))) * 43758.5453);
}

void main()
{
    frag_position = v_position;
    frag_normal = v_normal;
    frag_color = Color; // Original version
    frag_color.x += (0.1f * (100 - Health) / 20);
    frag_texture = v_texture;

    vec3 newPosition = v_position;

    if (isDying) {
        newPosition.x += rand3D(newPosition) * abs(sin(Time / 100000)) * 0.4f;
        newPosition.y += rand3D(newPosition) * abs(cos(Time / 100000)) * 0.4f;
        newPosition.z += rand3D(newPosition) * abs(sin(Time / 100000)) * 0.4f;
    }

    gl_Position = Projection * View * Model * vec4(newPosition, 1);
}
