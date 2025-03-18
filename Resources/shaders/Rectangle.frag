#version 410 core

in vec2 v_TexCoord;

// 基本參數
uniform vec2 u_Dimensions;    // 矩形的寬高比例
uniform float u_Thickness;    // 矩形的粗細 (實心為0或負值)
uniform float u_Rotation;     // 矩形的旋轉角度 (弧度)
uniform vec4 u_Color;         // 矩形的基本顏色
uniform float u_Time;         // 用於動畫效果

// 填充修飾器參數
uniform int u_FillType;       // 0=實心, 1=空心
uniform float u_FillThickness; // 空心時的線條粗細

// 邊緣修飾器參數
uniform int u_EdgeType;       // 0=無邊緣效果, 1=邊緣加深, 2=邊緣發光
uniform float u_EdgeWidth;    // 邊緣寬度
uniform vec4 u_EdgeColor;     // 邊緣顏色

// 動畫修飾器參數
uniform int u_AnimType;       // 0=無動畫, 1=波紋, 2=尾跡
uniform float u_Intensity;    // 效果強度
uniform float u_AnimSpeed;    // 效果速度

out vec4 fragColor;

// 應用旋轉到坐標
vec2 rotate2D(vec2 coord, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    mat2 rotMat = mat2(c, -s, s, c);
    return rotMat * coord;
}

void main() {
    // 旋轉坐標
    vec2 rotatedCoord = rotate2D(v_TexCoord, u_Rotation);

    // 計算矩形網格
    vec2 halfDim = u_Dimensions * 0.5;

    // 檢查是否在矩形範圍內
    bool insideX = abs(rotatedCoord.x) < halfDim.x;
    bool insideY = abs(rotatedCoord.y) < halfDim.y;

    // 對於空心矩形，計算內部邊界
    vec2 innerHalfDim = vec2(0);
    bool insideInnerX = false;
    bool insideInnerY = false;

    // 根據 FillModifier 選擇使用哪種粗細值
    float thickness = u_Thickness;
    if (u_FillType == 1) { // 如果是空心的，使用 FillModifier 的粗細值
        thickness = u_FillThickness;
    }

    if (thickness > 0.0) {
        // 調整為內部尺寸
        innerHalfDim = halfDim - vec2(thickness);

        // 如果內部尺寸有效，則檢查坐標是否在內部
        if (innerHalfDim.x > 0.0 && innerHalfDim.y > 0.0) {
            insideInnerX = abs(rotatedCoord.x) < innerHalfDim.x;
            insideInnerY = abs(rotatedCoord.y) < innerHalfDim.y;
        }
    }

    // 確定是否在矩形區域內
    bool insideRect = insideX && insideY;

    // 確定是否在內部區域內（對於空心矩形）
    bool insideInner = insideInnerX && insideInnerY && (thickness > 0.0);

    // 根據 FillType 決定是否渲染
    bool shouldRender = false;
    if (u_FillType == 0) { // 實心
        shouldRender = insideRect;
    } else { // 空心
        shouldRender = insideRect && !insideInner;
    }

    // 如果在矩形範圍內但不在內部範圍內（或矩形是實心的），則渲染
    if (shouldRender) {
        // 計算邊緣效果
        float edgeFactor = 0.0;

        if (u_EdgeType > 0) {
            // 計算到邊緣的距離
            float edgeDistX = halfDim.x - abs(rotatedCoord.x);
            float edgeDistY = halfDim.y - abs(rotatedCoord.y);
            float edgeDist = min(edgeDistX, edgeDistY);

            // 內部邊緣
            if (insideInner) {
                float innerEdgeDistX = abs(rotatedCoord.x) - innerHalfDim.x;
                float innerEdgeDistY = abs(rotatedCoord.y) - innerHalfDim.y;
                edgeDist = min(edgeDist, min(innerEdgeDistX, innerEdgeDistY));
            }

            edgeFactor = 1.0 - smoothstep(0.0, u_EdgeWidth, edgeDist);
        }

        // 基本顏色
        vec4 finalColor = u_Color;

        // 應用邊緣效果
        if (u_EdgeType == 1) { // 邊緣加深
            finalColor = mix(finalColor, vec4(0.0, 0.0, 0.0, finalColor.a), edgeFactor * 0.7);
        } else if (u_EdgeType == 2) { // 邊緣發光
            finalColor = mix(finalColor, u_EdgeColor, edgeFactor);
            finalColor.rgb *= 1.0 + edgeFactor * 2.0; // 讓邊緣更亮
        }

        // 波紋動畫效果
        if (u_AnimType == 1) {
            float wave = sin(u_Time * u_AnimSpeed * 3.0);
            wave = wave * 0.5 + 0.5; // 轉換到 0-1 範圍
            finalColor.rgb *= 1.0 + wave * u_Intensity * 0.2;
        }

        // 尾跡效果
        if (u_AnimType == 2) {
            vec2 direction = vec2(1.0, 0.0); // 預設向右
            float trail = smoothstep(0.0, 0.5, dot(normalize(rotatedCoord), -direction) * 0.5 + 0.5) *
                (1.0 - length(rotatedCoord) / length(halfDim));

            finalColor.rgb = mix(finalColor.rgb, finalColor.rgb * 1.5, trail * u_Intensity);
        }

        fragColor = finalColor;
    } else {
        // 不在矩形區域內，丟棄像素
        discard;
    }
}