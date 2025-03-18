#version 410 core

in vec2 v_TexCoord;  // 接收來自頂點著色器的 UV 坐標

out vec4 FragColor;

uniform vec4 u_Color;  // 血條顏色

void main() {
    // 根據指定的顏色來渲染血條
    FragColor = u_Color;
}
