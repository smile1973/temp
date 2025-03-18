// src/Attack/CircleAttack.cpp
#include "Attack/CircleAttack.hpp"
#include "Effect/EffectManager.hpp"
#include "Util/Logger.hpp"
#include <cmath>

CircleAttack::CircleAttack(const glm::vec2& position, float delay, float radius, int sequenceNumber)
    : Attack(position, delay, sequenceNumber),
      m_Radius(radius),
      m_Color(Util::Color::FromRGB(255, 0, 0, 128)) {
    // 注意: 所有特效現在都會在狀態轉換時創建，而不是在構造函數中
}

void CircleAttack::CreateWarningEffect() {
    LOG_DEBUG("CircleAttack::CreateWarningEffect called for position ({}, {}), radius: {}",
             m_Position.x, m_Position.y, m_Radius);

    try {
        // 獲取圓形警告特效
        auto warningEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::ENEMY_ATTACK_2);

        // 配置特效參數
        if (auto circleShape = std::dynamic_pointer_cast<Effect::Shape::CircleShape>(warningEffect->GetBaseShape())) {
            // 調整為適合半徑的相對尺寸
            float normalizedRadius = 0.35f;  // 預設歸一化半徑
            circleShape->SetRadius(normalizedRadius);

            // 設置視覺大小以匹配實際半徑
            float visualSize = m_Radius * 2.5f;
            circleShape->SetSize({visualSize, visualSize});

            // 設置顏色 - 半透明紅色用於警告
            circleShape->SetColor(Util::Color(1.0, 0.0, 0.0, 0.2));
            circleShape->SetDuration(m_Delay + 1.0f);
        }

        // 設置填充與邊緣效果
        warningEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));

        // 設置持續時間和播放特效
        warningEffect->SetDuration(m_Delay + 1.0f);
        warningEffect->Play(m_Position, m_ZIndex - 0.1f);

        m_WarningEffect = warningEffect;
        LOG_DEBUG("Circle warning effect created and played");
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in CreateWarningEffect: {}", e.what());
    }
}

void CircleAttack::CreateAttackEffect() {
    LOG_DEBUG("CircleAttack::CreateAttackEffect called");

    try {
        // 獲取圓形攻擊特效
        auto circleEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::ENEMY_ATTACK_2);

        // 配置特效參數
        if (auto circleShape = std::dynamic_pointer_cast<Effect::Shape::CircleShape>(circleEffect->GetBaseShape())) {
            // 調整為適合半徑的相對尺寸
            float normalizedRadius = 0.35f;
            circleShape->SetRadius(normalizedRadius);

            // 設置視覺大小以匹配實際半徑
            float visualSize = m_Radius * 2.5f;
            circleShape->SetSize({visualSize, visualSize});

            // 設置顏色
            circleShape->SetColor(m_Color);
        }

        // 設置填充與邊緣效果
        circleEffect->SetFillModifier(Effect::Modifier::FillModifier(
            m_UseGlowEffect ? Effect::Modifier::FillType::HOLLOW : Effect::Modifier::FillType::SOLID,
            0.03f
        ));

        if (m_UseGlowEffect) {
            circleEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(
                Effect::Modifier::EdgeType::GLOW,
                0.08f,
                Util::Color::FromRGB(255, 50, 50, 255)
            ));
        } else {
            circleEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(
                Effect::Modifier::EdgeType::DARK,
                0.05f
            ));
        }

        // 設置持續時間和播放特效
        circleEffect->SetDuration(m_AttackDuration);
        circleEffect->Play(m_Position, m_ZIndex + 0.2f);

        m_AttackEffect = circleEffect;
        LOG_DEBUG("Circle attack effect created and played");
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in CreateAttackEffect: {}", e.what());
    }
}

bool CircleAttack::CheckCollisionInternal(const std::shared_ptr<Character>& character) {
    // 獲取角色位置
    glm::vec2 characterPos = character->GetPosition();

    // 計算角色與攻擊中心的距離
    float distance = glm::length(characterPos - m_Position);

    // 如果距離小於半徑，則判定為碰撞
    return distance <= m_Radius;
}