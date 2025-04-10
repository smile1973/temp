// src/Attack/RectangleAttack.cpp
#include "Attack/RectangleAttack.hpp"
#include "Effect/EffectManager.hpp"
#include "Util/Logger.hpp"
#include <cmath>
#include "App.hpp"

std::shared_ptr<Util::Image> RectangleAttack::s_ClockwiseImage = nullptr;
std::shared_ptr<Util::Image> RectangleAttack::s_CounterClockwiseImage = nullptr;

RectangleAttack::RectangleAttack(const glm::vec2& position, float delay,
                               float width, float height,
                               float rotation, int sequenceNumber)
    : Attack(position, delay, sequenceNumber),
      m_Width(width),
      m_Height(height),
      m_Rotation(rotation),
      m_Color(Util::Color::FromRGB(255, 50, 0, 150)) {}

RectangleAttack::RectangleAttack(const glm::vec2& position, float delay, Direction direction,
                               float width, float height, int sequenceNumber)
    : Attack(position, delay, sequenceNumber),
      m_Width(width),
      m_Height(height),  // 使用length作為高度
      m_Direction(direction),
      m_Color(Util::Color::FromRGB(255, 20, 20, 200)) {
    m_Rotation = CalculateRotationAngle();
    m_UseGlowEffect = true;
}

void RectangleAttack::SetRotation(float rotation) {
    m_Rotation = rotation;

    if (m_AttackEffect) {
        if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(m_AttackEffect->GetBaseShape())) {
            rectangleShape->SetRotation(rotation);
        }
    }
}

void RectangleAttack::CreateWarningEffect() {
    // LOG_DEBUG("RectangleAttack::CreateWarningEffect called for position ({}, {}), size: {}x{}, rotation: {}",
    //          m_Position.x, m_Position.y, m_Width, m_Height, m_Rotation);

    try {
        auto warningEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_BEAM);
        if (!warningEffect) return;

        // 特效參數
        if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(warningEffect->GetBaseShape())) {
            // 計算歸一化的尺寸比例
            float maxDimension = std::max(m_Width, m_Height);
            float normalizedWidth = m_Width / maxDimension;
            float normalizedHeight = m_Height / maxDimension;

            rectangleShape->SetDimensions({normalizedWidth, normalizedHeight});
            rectangleShape->SetRotation(m_Rotation);

            // 禁用旋轉
            rectangleShape->SetAutoRotation(false);
            rectangleShape->SetSize({maxDimension * 1.2f, maxDimension * 1.2f});
            rectangleShape->SetColor(Util::Color(0.9, 0.1, 0.1, 0.5));

            // LOG_DEBUG("Rectangle warning effect shape configured");
        } else {
            LOG_ERROR("Failed to cast to RectangleShape");
        }

        // 設置填充與邊緣效果
        warningEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));
        warningEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(Effect::Modifier::EdgeType::GLOW, 0.001, Util::Color(0.9, 0.1, 0.1, 0.7)));

        // 關閉任何動畫
        warningEffect->SetAnimationModifier(Effect::Modifier::AnimationModifier(
            Effect::Modifier::AnimationType::NONE,
            0.0f,
            0.0f
        ));

        warningEffect->SetDuration(m_Delay + 1.0f);  // 確保持續足夠長的時間
        warningEffect->Play(m_Position, m_ZIndex + 0.1f);

        m_WarningEffect = warningEffect;
        // LOG_DEBUG("Rectangle warning effect created and played");
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in CreateWarningEffect: {}", e.what());
    }
}

void RectangleAttack::CreateAttackEffect() {
    // LOG_DEBUG("RectangleAttack::CreateAttackEffect called with duration: {}", m_AttackDuration);

    try {
        auto rectangleEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_LASER);

        if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(rectangleEffect->GetBaseShape())) {
            // 計算歸一化的尺寸比例
            float maxDimension = std::max(m_Width, m_Height);
            float normalizedWidth = m_Width / maxDimension;
            float normalizedHeight = m_Height / maxDimension;

            rectangleShape->SetDimensions({normalizedWidth, normalizedHeight});
            rectangleShape->SetRotation(m_Rotation);

            rectangleShape->SetAutoRotation(m_AutoRotate, m_RotationSpeed);

            rectangleShape->SetSize({maxDimension, maxDimension});
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

        // 使用較大值來確保特效持續整個攻擊階段
        float effectDuration = m_AttackDuration * 1.5f;
        rectangleEffect->SetDuration(effectDuration);

        rectangleEffect->Play(m_Position, m_ZIndex - 2.0f);
        m_AttackEffect = rectangleEffect;
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in CreateAttackEffect: {}", e.what());
    }
}

float RectangleAttack::CalculateRotationAngle() const {
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

bool RectangleAttack::CheckCollisionInternal(const std::shared_ptr<Character>& character) {
    glm::vec2 characterPos = character->GetPosition();
    // LOG_DEBUG("Character pos: {}", characterPos);
    return IsPointInRectangle(characterPos);
}

bool RectangleAttack::IsPointInRectangle(const glm::vec2& circleCenter) const {
    // 計算矩形四個角在旋轉後的世界座標
    float halfWidth = m_Width / 2.0f;
    float halfHeight = m_Height / 2.0f;

    // 未旋轉時的四個角（相對於矩形中心）
    glm::vec2 corners[4] = {
        {-halfWidth, -halfHeight},  // 左下
        {halfWidth, -halfHeight},   // 右下
        {halfWidth, halfHeight},    // 右上
        {-halfWidth, halfHeight}    // 左上
    };

    // 旋轉矩陣
    float cosA = cos(m_Rotation);
    float sinA = sin(m_Rotation);

    // LOG_DEBUG("m_Rotation : {}", m_Rotation);

    glm::vec2 rotatedCorners[4];
    for (int i = 0; i < 4; i++) {
        rotatedCorners[i].x = corners[i].x * cosA + corners[i].y * sinA + m_Position.x;
        rotatedCorners[i].y = -corners[i].x * sinA + corners[i].y * cosA + m_Position.y;
    }

    bool inside = IsPointInPolygon(circleCenter, rotatedCorners, 4);
    if (inside) {
        return true;
    }

    return false;
}

bool RectangleAttack::IsPointInPolygon(const glm::vec2& point, const glm::vec2* vertices, int vertexCount) const{
    bool inside = false;
    for (int i = 0, j = vertexCount - 1; i < vertexCount; j = i++) {
        if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
            (point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) /
             (vertices[j].y - vertices[i].y) + vertices[i].x)) {
            inside = !inside;
             }
    }
    return inside;
}

void RectangleAttack::SyncWithEffect() {
    // 檢查攻擊特效是否存在且處於活躍狀態
    if (m_AttackEffect && m_AttackEffect->IsActive()) {
        // 嘗試獲取矩形形狀
        auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(m_AttackEffect->GetBaseShape());
        if (rectangleShape) {
            // 從特效獲取當前旋轉角度
            float currentRotation = rectangleShape->GetRotation();

            // 更新攻擊的旋轉角度，用於碰撞檢測
            m_Rotation = currentRotation;
        }
    }
}

void RectangleAttack::CreateDirectionIndicator() {
    // 懶初始化圖片資源
    if (!s_ClockwiseImage) s_ClockwiseImage = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/Image/clockwise.png");
    if (!s_CounterClockwiseImage) s_CounterClockwiseImage = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/Image/cclockwise.png");

    if (!s_ClockwiseImage || !s_CounterClockwiseImage) {
        LOG_ERROR("Direction indicator images not available");
        return;
    }

    // 根據旋轉速度選擇適當的圖片
    std::shared_ptr<Util::Image> directionImage =
        (m_RotationSpeed > 0) ? s_ClockwiseImage : s_CounterClockwiseImage;

    // 創建顯示圖片的遊戲物件
    m_DirectionIndicator = std::make_shared<Util::GameObject>(
        directionImage,        // 使用選擇的圖片
        m_ZIndex + 0.5f,       // 確保在攻擊和時間條之上
        m_Position,
        true                   // 可見
    );

    // 縮放圖片到合適大小
    m_DirectionIndicator->m_Transform.scale = glm::vec2(1.0f, 1.0f);
    // 將方向指示器添加為子物件
    App::GetInstance().AddToRoot(m_DirectionIndicator);
    LOG_DEBUG("Direction indicator created for rotation speed: {}", m_RotationSpeed);
}

void RectangleAttack::OnCountdownStart() {
    // 呼叫基類的方法來創建時間條
    Attack::OnCountdownStart();

    // 創建方向指示器
    if (m_AutoRotate) {
        CreateDirectionIndicator();
    }
}

void RectangleAttack::OnAttackStart() {
    Attack::OnAttackStart();

    if (m_DirectionIndicator) {
        App::GetInstance().RemoveFromRoot(m_DirectionIndicator);
        m_DirectionIndicator = nullptr;
    }
}

void RectangleAttack::OnCountdownUpdate(float deltaTime) {
    // 呼叫基類的更新方法
    Attack::OnCountdownUpdate(deltaTime);

    if (m_DirectionIndicator && m_AutoRotate) {
        // 旋轉方向與雷射相同
        m_DirectionIndicator->m_Transform.rotation += deltaTime * m_RotationSpeed * -15.0f;
    }
}

void RectangleAttack::CleanupVisuals() {
    Attack::CleanupVisuals();
    if (m_DirectionIndicator) {
        App::GetInstance().RemoveFromRoot(m_DirectionIndicator);
        m_DirectionIndicator = nullptr;
    }
}