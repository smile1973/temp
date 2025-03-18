#include "Effect/Shape/BaseShape.hpp"
#include "Util/Logger.hpp"

namespace Effect {
    namespace Shape {

        BaseShape::BaseShape(float duration) {
            m_Duration = duration;
        }

        void BaseShape::Update(float deltaTime) {
            if (m_State == State::ACTIVE) {
                // LOG_DEBUG("m_duration = {} ", m_Duration);
                m_ElapsedTime += deltaTime;
                if (m_ElapsedTime >= m_Duration) {
                    m_State = State::FINISHED;
                }
            }
        }

        void BaseShape::Play(const glm::vec2& position, float zIndex) {
            Reset();
            m_Transform.translation = position;
            m_ZIndex = zIndex;
            m_State = State::ACTIVE;
        }

        void BaseShape::Reset() {
            m_ElapsedTime = 0.0f;
            m_State = State::INACTIVE;
        }

    } // namespace Shape
} // namespace Effect