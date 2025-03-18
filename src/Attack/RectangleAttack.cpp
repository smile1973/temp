// src/Attack/RectangleAttack.cpp
#include "Attack/RectangleAttack.hpp"
#include "Effect/EffectManager.hpp"
#include "Util/Logger.hpp"
#include <cmath>

RectangleAttack::RectangleAttack(const glm::vec2& position, float delay,
                               float width, float height,
                               float rotation, int sequenceNumber)
    : Attack(position, delay, sequenceNumber),
      m_Width(width),
      m_Height(height),
      m_Rotation(rotation),
      m_Color(Util::Color::FromRGB(255, 50, 0, 150)) {
    // 注意: 所有特效現在都會在狀態轉換時創建，而不是在建構函數中
}

void RectangleAttack::CreateWarningEffect() {
    LOG_DEBUG("RectangleAttack::CreateWarningEffect called for position ({}, {}), size: {}x{}, rotation: {}",
             m_Position.x, m_Position.y, m_Width, m_Height, m_Rotation);

    try {
        // 獲取矩形警告特效 - 使用RECT_BEAM，這已經在CircleAttack中確認可行
        auto warningEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_BEAM);

        if (!warningEffect) {
            LOG_ERROR("Failed to get rectangle warning effect");
            return;
        }

        // 配置特效參數
        if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(warningEffect->GetBaseShape())) {
            // 計算歸一化的尺寸比例
            float maxDimension = std::max(m_Width, m_Height);
            float normalizedWidth = m_Width / maxDimension;
            float normalizedHeight = m_Height / maxDimension;

            rectangleShape->SetDimensions({normalizedWidth, normalizedHeight});
            rectangleShape->SetRotation(m_Rotation);

            // 禁用自動旋轉
            rectangleShape->SetAutoRotation(false);

            // 設置視覺大小
            rectangleShape->SetSize({maxDimension * 1.2f, maxDimension * 1.2f});

            // 設置顏色 - 半透明紅色用於警告
            rectangleShape->SetColor(Util::Color(0.9, 0.1, 0.1, 0.4));

            LOG_DEBUG("Rectangle warning effect shape configured");
        } else {
            LOG_ERROR("Failed to cast to RectangleShape");
        }

        // 設置填充與邊緣效果
        warningEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));

        // 關閉任何動畫
        warningEffect->SetAnimationModifier(Effect::Modifier::AnimationModifier(
            Effect::Modifier::AnimationType::NONE,
            0.0f,
            0.0f
        ));

        // 設置持續時間和播放特效
        warningEffect->SetDuration(m_Delay + 1.0f);  // 確保持續足夠長的時間
        warningEffect->Play(m_Position, m_ZIndex + 0.1f);

        m_WarningEffect = warningEffect;
        LOG_DEBUG("Rectangle warning effect created and played");
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in CreateWarningEffect: {}", e.what());
    }
}

void RectangleAttack::CreateAttackEffect() {
    LOG_DEBUG("RectangleAttack::CreateAttackEffect called");

    try {
        // 獲取矩形攻擊特效
        auto rectangleEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_LASER);

        // 配置特效參數
        if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(rectangleEffect->GetBaseShape())) {
            // 計算歸一化的尺寸比例
            float maxDimension = std::max(m_Width, m_Height);
            float normalizedWidth = m_Width / maxDimension;
            float normalizedHeight = m_Height / maxDimension;

            rectangleShape->SetDimensions({normalizedWidth, normalizedHeight});
            rectangleShape->SetRotation(m_Rotation);

            // 禁用自動旋轉
            rectangleShape->SetAutoRotation(false);

            // 設置視覺大小
            rectangleShape->SetSize({maxDimension * 1.2f, maxDimension * 1.2f});

            // 設置顏色
            // rectangleShape->SetColor(m_Color);
            rectangleShape->SetColor(Util::Color(0.9, 0.7, 0.3, 0.4));
        }

        // 設置填充與邊緣效果
        rectangleEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));

        rectangleEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(Effect::Modifier::EdgeType::GLOW, 0.001, Util::Color(0.9, 0.1, 0.1, 0.7)));

        // 關閉任何動畫
        rectangleEffect->SetAnimationModifier(Effect::Modifier::AnimationModifier(
            Effect::Modifier::AnimationType::NONE,
            0.0f,
            0.0f
        ));

        // 設置持續時間和播放特效
        rectangleEffect->SetDuration(m_AttackDuration);
        rectangleEffect->Play(m_Position, m_ZIndex + 0.2f);

        m_AttackEffect = rectangleEffect;
        LOG_DEBUG("Rectangle attack effect created and played");
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in CreateAttackEffect: {}", e.what());
    }
}

bool RectangleAttack::CheckCollisionInternal(const std::shared_ptr<Character>& character) {
    // 獲取角色位置
    glm::vec2 characterPos = character->GetPosition();

    // 檢查點是否在旋轉後的矩形內
    return IsPointInRectangle(characterPos);
}

bool RectangleAttack::IsPointInRectangle(const glm::vec2& point) const {
    // 計算點相對於矩形中心的位置
    glm::vec2 relativePoint = point - m_Position;

    // 計算旋轉的逆變換，將點轉回矩形的本地坐標系
    float cosAngle = cos(-m_Rotation);
    float sinAngle = sin(-m_Rotation);

    glm::vec2 rotatedPoint = {
        relativePoint.x * cosAngle - relativePoint.y * sinAngle,
        relativePoint.x * sinAngle + relativePoint.y * cosAngle
    };

    // 檢查點是否在矩形範圍內（以原點為中心的矩形）
    float halfWidth = m_Width / 2.0f;
    float halfHeight = m_Height / 2.0f;

    return (rotatedPoint.x >= -halfWidth && rotatedPoint.x <= halfWidth &&
            rotatedPoint.y >= -halfHeight && rotatedPoint.y <= halfHeight);
}