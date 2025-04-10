#version 410 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform float u_Width;
uniform vec2 u_Position;  // 新增血條位置

void main() {
    gl_Position = vec4(u_Position.x + a_Position.x * u_Width,
                       u_Position.y + a_Position.y,
                       0.0, 1.0);
    v_TexCoord = a_TexCoord;
}
