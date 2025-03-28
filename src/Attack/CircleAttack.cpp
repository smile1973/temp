#include "Attack/CircleAttack.hpp"
#include "Effect/EffectManager.hpp"
#include "Util/Logger.hpp"
#include <cmath>

CircleAttack::CircleAttack(const glm::vec2& position, float delay, float radius, int sequenceNumber)
    : Attack(position, delay, sequenceNumber),
      m_Radius(radius),
      m_Color(Util::Color(1.0, 1.0, 1.0, 0.3)) {
}

void CircleAttack::CreateWarningEffect() {
    // LOG_DEBUG("CircleAttack::CreateWarningEffect called for position ({}, {}), radius: {}",
    //          m_Position.x, m_Position.y, m_Radius);

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
        warningEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(Effect::Modifier::EdgeType::GLOW, 0.005, Util::Color(1.0, 0.0, 0.0, 0.7)));

        // 設置持續時間和播放特效
        warningEffect->SetDuration(m_Delay + 1.0f);
        warningEffect->Play(m_Position, m_ZIndex - 0.1f);

        m_WarningEffect = warningEffect;
        // LOG_DEBUG("Circle warning effect created and played");
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in CreateWarningEffect: {}", e.what());
    }
}

// 修改 CircleAttack.cpp 中的 CreateAttackEffect 方法來確保移動參數正確應用
void CircleAttack::CreateAttackEffect() {
    // LOG_DEBUG("CircleAttack::CreateAttackEffect called with isMoving: {}", m_IsMoving);

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
        circleEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));
        circleEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(Effect::Modifier::EdgeType::GLOW, 0.005, Util::Color(1.0, 0.0, 0.0, 0.7)));

        // 如果設置了移動，那麼添加移動修飾器
        if (m_IsMoving) {
            // 計算基於速度和距離的持續時間
            float moveDuration = m_Distance / m_Speed;

            // 確保攻擊持續時間足夠長以完成整個移動
            m_AttackDuration = std::max(m_AttackDuration, moveDuration);

            LOG_DEBUG("Setting up movement with speed: {}, distance: {}, duration: {}",
                      m_Speed, m_Distance, moveDuration);

            // 設置移動修飾器
            auto movementMod = Effect::Modifier::MovementModifier(
                true, m_Speed, m_Distance, m_Direction
            );

            // 明確設置起始位置，確保從正確位置開始移動
            movementMod.SetStartPosition(m_Position);

            circleEffect->SetMovementModifier(movementMod);
        }

        // 設置持續時間和播放特效
        circleEffect->SetDuration(m_AttackDuration);
        circleEffect->Play(m_Position, m_ZIndex + 0.2f);

        // LOG_DEBUG("Circle attack effect created at ({}, {}) with duration: {}",
        //           m_Position.x, m_Position.y, m_AttackDuration);

        m_AttackEffect = circleEffect;
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in CreateAttackEffect: {}", e.what());
    }
}

bool CircleAttack::CheckCollisionInternal(const std::shared_ptr<Character>& character) {
    glm::vec2 characterPos = character->GetPosition();
    float distance = glm::length(characterPos - m_Position);

    // 如果距離小於半徑，則判定為碰撞
    return distance <= m_Radius;
}

void CircleAttack::SyncWithEffect() {
    // 檢查攻擊特效是否存在且處於活躍狀態
    if (m_AttackEffect && m_AttackEffect->IsActive()) {
        // 獲取特效當前位置
        glm::vec2 effectPosition = m_AttackEffect->GetPosition();

        // 更新攻擊的位置，用於碰撞檢測
        m_Position = effectPosition;

        // 同時更新 GameObject 的位置
        m_Transform.translation = effectPosition;
    }
}