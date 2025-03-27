#version 410 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

uniform Matrices {
    mat4 model;
    mat4 projection;
};

out vec2 v_TexCoord;

void main() {
    gl_Position = projection * model * vec4(position, 0.0, 1.0);
    v_TexCoord = texCoord - vec2(0.5, 0.5); // 將UV坐標移到中心
}