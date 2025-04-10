#ifndef MOVEMENTMODIFIER_HPP
#define MOVEMENTMODIFIER_HPP

#include "pch.hpp"

namespace Effect {
    namespace Modifier {

        class MovementModifier {
        public:
            MovementModifier(bool isMoving = false, float speed = 200.0f, float distance = 400.0f, const glm::vec2& direction = {1.0f, 0.0f});

            // 更新位置
            void Update(float deltaTime, glm::vec2& position);

            // 設置參數
            void SetMoving(bool isMoving) { m_IsMoving = isMoving; }
            bool IsMoving() const { return m_IsMoving; }

            void SetSpeed(float speed) { m_Speed = speed; }
            float GetSpeed() const { return m_Speed; }

            void SetDistance(float distance) { m_MaxDistance = distance; }
            float GetDistance() const { return m_MaxDistance; }

            void SetDirection(const glm::vec2& direction) { m_Direction = glm::normalize(direction); }
            const glm::vec2& GetDirection() const { return m_Direction; }

            void SetStartPosition(const glm::vec2& position) { m_StartPosition = position; }
            // 重置移動狀態
            void Reset();

            // 檢查是否已達到最大距離
            bool HasReachedDestination() const;

        private:
            bool m_IsMoving;                // 是否移動
            float m_Speed;                  // 移動速度
            float m_MaxDistance;            // 最大移動距離
            glm::vec2 m_Direction;          // 移動方向

            glm::vec2 m_StartPosition;      // 起始位置
            float m_DistanceTraveled = 0.0f; // 已移動距離
        };

    } // namespace Modifier
} // namespace Effect

#endif //MOVEMENTMODIFIER_HPP
