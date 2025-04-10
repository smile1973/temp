#include "Character.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/TransformUtils.hpp"

Object::Object(const std::string& ImagePath) {
    SetImage(ImagePath);
    m_Transform.scale = glm::vec2(0.5f, 0.5f);

    ResetPosition();
}

void Object::SetImage(const std::string& ImagePath) {
    m_ImagePath = ImagePath;

    m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}

void Object::Update() {
    // 移動位置
    if (m_IsMoving) {
        // 計算移動距離
        const float DeltaTimeMs = Util::Time::GetDeltaTimeMs();
        m_TotalTime -= DeltaTimeMs;
        // 更新位置
        m_Transform.translation += m_MoveSpeed * DeltaTimeMs / 1000.0f;
        if (m_TotalTime < 0.0f) {
            m_IsMoving = false; // 停止移動
            m_Transform.translation = m_TargetPosition;
            m_TotalTime = 0;
        }
    }
}



void Object::MovePosition(const glm::vec2& Position, const float totalTime) {
    MoveToPosition(GetPosition() + Position, totalTime);
}

void Object::MoveToPosition(const glm::vec2& targetPosition, const float totalTime) {
    if (totalTime <= 0.0f) {
        m_Transform.translation = targetPosition;
        return;
    }

    m_IsMoving = true;
    m_TargetPosition = targetPosition;
    m_MoveSpeed = (targetPosition - this->GetPosition()) / totalTime;
    m_TotalTime = totalTime * 1000.0f; //(ms)
}