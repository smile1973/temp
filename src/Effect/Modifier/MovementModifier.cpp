#include "Effect/Modifier/MovementModifier.hpp"

namespace Effect {
    namespace Modifier {

        MovementModifier::MovementModifier(bool isMoving, float speed, float distance, const glm::vec2& direction)
            : m_IsMoving(isMoving), m_Speed(speed), m_MaxDistance(distance), m_Direction(glm::normalize(direction)) {
        }

        void MovementModifier::Update(float deltaTime, glm::vec2& position) {
            if (!m_IsMoving) return;

            // 計算移動距離
            float moveDistance = m_Speed * deltaTime;
            m_DistanceTraveled += moveDistance;

            // 更新位置
            position = m_StartPosition + m_Direction * m_DistanceTraveled;

            // 檢查是否到達目標距離
            if (m_DistanceTraveled >= m_MaxDistance) {
                m_IsMoving = false; // 停止移動
            }
        }

        void MovementModifier::Reset() {
            m_DistanceTraveled = 0.0f;
        }


        bool MovementModifier::HasReachedDestination() const {
            return !m_IsMoving || m_DistanceTraveled >= m_MaxDistance;
        }

    } // namespace Modifier
} // namespace Effect