// src/LaserAttack.cpp
#include "Attack/LaserAttack.hpp"
#include "Effect/EffectManager.hpp"
#include "Util/Logger.hpp"
#include <cmath>

LaserAttack::LaserAttack(const glm::vec2& position, float delay, Direction direction,
                         float width, float length, int sequenceNumber)
    : Attack(position, delay, sequenceNumber),
      m_Direction(direction),
      m_Width(width),
      m_Length(length) {

    // 計算旋轉角度
    m_RotationAngle = CalculateRotationAngle();

    // 立即創建警告效果
    CreateWarningEffect();
}

void LaserAttack::SetDirection(Direction direction) {
    m_Direction = direction;
    m_RotationAngle = CalculateRotationAngle();

    // 如果已有警告或攻擊效果，更新它們的旋轉
    if (m_WarningEffect || m_AttackEffect) {
        // 需要重新創建效果
        if (m_State == State::WARNING) {
            CreateWarningEffect();
        } else if (m_State == State::ATTACKING) {
            CreateAttackEffect();
        }
    }
}

void LaserAttack::CreateWarningEffect() {
    // 使用矩形效果創建雷射警告
    auto rectangleEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_BEAM);

    // 設置雷射警告的屬性
    if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(rectangleEffect->GetBaseShape())) {
        // 設置寬高比 - 根據雷射長寬比例
        rectangleShape->SetDimensions(glm::vec2(m_Length / 1000.0f, m_Width / 1000.0f));

        // 設置旋轉角度
        rectangleShape->SetRotation(m_RotationAngle);

        // 禁用自動旋轉
        rectangleShape->SetAutoRotation(false);

        // 設置尺寸
        rectangleShape->SetSize(glm::vec2(1000, 1000));
    }

    // 設置填充與邊緣效果
    rectangleEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));
    rectangleEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(
        Effect::Modifier::EdgeType::GLOW,
        0.04f,
        Util::Color::FromRGB(255, 0, 0, 150)
    ));

    // 設置警告顏色 - 半透明紅色
    rectangleEffect->GetBaseShape()->SetColor(Util::Color::FromRGB(255, 0, 0, 80));

    // 設置持續時間
    rectangleEffect->SetDuration(m_Delay + 0.5f);  // 警告持續到攻擊結束

    // 播放警告效果
    rectangleEffect->Play(m_Position, m_ZIndex - 1.0f);

    m_WarningEffect = rectangleEffect;
}

void LaserAttack::CreateAttackEffect() {
    // 使用矩形效果創建雷射攻擊
    auto rectangleEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_LASER);

    // 設置雷射攻擊的屬性
    if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(rectangleEffect->GetBaseShape())) {
        // 設置寬高比 - 根據雷射長寬比例
        rectangleShape->SetDimensions(glm::vec2(m_Length / 1000.0f, m_Width / 1000.0f));

        // 設置旋轉角度
        rectangleShape->SetRotation(m_RotationAngle);

        // 禁用自動旋轉
        rectangleShape->SetAutoRotation(false);

        // 設置尺寸
        rectangleShape->SetSize(glm::vec2(1000, 1000));
    }

    // 設置填充與邊緣效果
    rectangleEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));
    rectangleEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(
        Effect::Modifier::EdgeType::GLOW,
        0.06f,
        Util::Color::FromRGB(255, 50, 50, 255)
    ));

    // 設置攻擊顏色 - 明亮紅色
    rectangleEffect->GetBaseShape()->SetColor(Util::Color::FromRGB(255, 20, 20, 200));

    // 設置持續時間
    rectangleEffect->SetDuration(m_AttackDuration);

    // 播放攻擊效果
    rectangleEffect->Play(m_Position, m_ZIndex + 0.2f);

    m_AttackEffect = rectangleEffect;
}

bool LaserAttack::CheckCollisionInternal(const std::shared_ptr<Character>& character) {
    // 獲取角色位置
    glm::vec2 characterPos = character->GetPosition();

    // 檢查角色是否在雷射範圍內
    return IsPointInLaser(characterPos);
}

float LaserAttack::CalculateRotationAngle() const {
    switch (m_Direction) {
        case Direction::HORIZONTAL:
            return 0.0f;  // 0度
        case Direction::VERTICAL:
            return 1.5708f;  // 90度（π/2）
        case Direction::DIAGONAL_TL_BR:
            return 0.7854f;  // 45度（π/4）
        case Direction::DIAGONAL_TR_BL:
            return -0.7854f;  // -45度（-π/4）
        default:
            return 0.0f;
    }
}

bool LaserAttack::IsPointInLaser(const glm::vec2& point) const {
    // 將點相對於雷射中心的坐標
    glm::vec2 relativePos = point - m_Position;

    // 將點旋轉回雷射的坐標系（逆旋轉）
    float cosAngle = cos(-m_RotationAngle);
    float sinAngle = sin(-m_RotationAngle);

    glm::vec2 rotatedPos = {
        relativePos.x * cosAngle - relativePos.y * sinAngle,
        relativePos.x * sinAngle + relativePos.y * cosAngle
    };

    // 檢查點是否在雷射的矩形區域內
    return (fabs(rotatedPos.x) <= m_Length / 2.0f) &&
           (fabs(rotatedPos.y) <= m_Width / 2.0f);
}