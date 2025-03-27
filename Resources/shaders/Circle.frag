#version 410 core

in vec2 v_TexCoord;

// 基本參數
uniform float u_Radius;     // 圓的半徑 (0.0-0.5)
uniform vec4 u_Color;       // 圓的基本顏色
uniform float u_Time;       // 用於動畫效果

// 填充修飾器參數
uniform int u_FillType;     // 0=實心, 1=空心
uniform float u_Thickness;  // 空心時的線條粗細
uniform float u_FillThickness; // 替代名稱，用於兼容性 (與 u_Thickness 相同)

// 邊緣修飾器參數
uniform int u_EdgeType;     // 0=無邊緣效果, 1=邊緣加深, 2=邊緣發光
uniform float u_EdgeWidth;  // 邊緣寬度
uniform vec4 u_EdgeColor;   // 邊緣顏色

// 動畫修飾器參數
uniform int u_AnimType;     // 0=無動畫, 1=波紋, 2=尾跡
uniform float u_Intensity;  // 效果強度
uniform float u_AnimSpeed;  // 效果速度

out vec4 fragColor;

void main() {
    float dist = length(v_TexCoord);

    // 基本圓形
    float circle = 1.0;

    // 波紋動畫效果
    float animEffect = 0.0;
    if (u_AnimType == 1) { // 波紋效果
        animEffect = u_Intensity * 0.1 * sin(u_Time * u_AnimSpeed * 3.0);
    }

    // 填充類型處理
    if (u_FillType == 0) { // 實心
        circle = 1.0 - smoothstep(u_Radius + animEffect - 0.01, u_Radius + animEffect, dist);
    } else { // 空心
        float inner = u_Radius - u_Thickness;
        float outer = u_Radius;
        circle = smoothstep(inner + animEffect - 0.01, inner + animEffect, dist) *
                 (1.0 - smoothstep(outer + animEffect - 0.01, outer + animEffect, dist));
    }

    // 丟棄非圓形區域
    if (circle < 0.01) {
        discard;
    }

    // 計算顏色
    vec4 finalColor = u_Color;

    // 邊緣效果
    if (u_EdgeType > 0) {
        float edge = 0.0;
        if (u_FillType == 0) { // 實心的邊緣
            edge = smoothstep(u_Radius + animEffect - u_EdgeWidth, u_Radius + animEffect, dist);
        } else { // 空心的邊緣
            float inner = u_Radius - u_Thickness;
            float outer = u_Radius;

            float innerEdge = smoothstep(inner + animEffect, inner + animEffect + u_EdgeWidth, dist);
            float outerEdge = smoothstep(outer + animEffect - u_EdgeWidth, outer + animEffect, dist);

            edge = innerEdge * (1.0 - outerEdge);
        }

        if (u_EdgeType == 1) { // 邊緣加深
            finalColor = mix(finalColor, vec4(0.0, 0.0, 0.0, finalColor.a), edge * 0.7);
        } else if (u_EdgeType == 2) { // 邊緣發光
            finalColor = mix(finalColor, u_EdgeColor, edge);
            finalColor.rgb *= 1.0 + edge * 2.0; // 讓邊緣更亮
        }
    }

    // 尾跡效果
    if (u_AnimType == 2) { // 尾跡效果
        vec2 direction = vec2(1.0, 0.0); // 假設向右
        float trail = smoothstep(0.0, 0.5, dot(normalize(v_TexCoord), -direction) * 0.5 + 0.5) *
                    (1.0 - dist / (u_Radius + animEffect));

        finalColor.rgb = mix(finalColor.rgb, finalColor.rgb * 1.5, trail * u_Intensity);
    }

    // 輸出最終顏色
    fragColor = finalColor;
}