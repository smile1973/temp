// Effect/CompositeEffect.cpp
#include "Effect/CompositeEffect.hpp"
#include "Util/Logger.hpp"

namespace Effect {
    CompositeEffect::CompositeEffect(const std::shared_ptr<Shape::BaseShape>& baseShape)
        : m_BaseShape(baseShape) {

        if (!baseShape) {
            LOG_ERROR("CompositeEffect created with null baseShape");
        }
    }

    void CompositeEffect::Draw(const Core::Matrices& data) {
        if (m_State != State::ACTIVE || !m_BaseShape) return;

        // First get program ID
        Core::Program* program = nullptr;

        if (auto circleShape = dynamic_cast<Shape::CircleShape*>(m_BaseShape.get())) {
            (void) circleShape;
            program = Shape::CircleShape::GetProgram();
        } else if (auto ellipseShape = dynamic_cast<Shape::EllipseShape*>(m_BaseShape.get())) {
            (void) ellipseShape;
            program = Shape::EllipseShape::GetProgram();
        } else if (auto rectangleShape = dynamic_cast<Shape::RectangleShape*>(m_BaseShape.get())) {
            (void) rectangleShape;
            program = Shape::RectangleShape::GetProgram();
        }

        if (!program) {
            LOG_ERROR("Failed to get program for CompositeEffect - BaseShape type not recognized");
            return;
        }

        // 先綁定程序，這樣才能設置 uniform
        program->Bind();

        // 依次應用修飾器
        m_FillModifier.Apply(*program);
        m_EdgeModifier.Apply(*program);
        m_AnimationModifier.Apply(*program, m_ElapsedTime);

        // 設置時間 uniform - 確保基本的時間變量存在於所有著色器中
        GLint timeLocation = glGetUniformLocation(program->GetId(), "u_Time");
        if (timeLocation != -1) {
            glUniform1f(timeLocation, m_ElapsedTime);
        }

        // 驗證著色器程序以確保所有 uniform 都已設置正確
        program->Validate();

        // 繪製基本形狀
        m_BaseShape->Draw(data);
    }

    glm::vec2 CompositeEffect::GetSize() const {
        return m_BaseShape ? m_BaseShape->GetSize() : glm::vec2(0.0f);
    }

    void CompositeEffect::Update(float deltaTime) {
        if (m_State != State::ACTIVE) return;

        // Update base shape
        m_BaseShape->Update(deltaTime);

        // Apply movement modifier
        if (m_MovementModifier.IsMoving()) {
            glm::vec2 position = m_Transform.translation;
            m_MovementModifier.Update(deltaTime, position);
            m_Transform.translation = position;

            // If reached destination and base shape is still active, end the effect
            if (m_MovementModifier.HasReachedDestination() && m_BaseShape->GetState() == State::ACTIVE) {
                m_BaseShape->Reset();
                m_State = State::FINISHED;
            }
        }

        // Sync state
        if (m_BaseShape->GetState() == State::FINISHED) {
            m_State = State::FINISHED;
        }

        // Update time
        m_ElapsedTime += deltaTime;
        if (m_ElapsedTime >= m_Duration) {
            m_State = State::FINISHED;
        }
    }

    void CompositeEffect::Play(const glm::vec2& position, float zIndex) {
        if (!m_BaseShape) return;
        Reset();
        m_Transform.translation = position;
        m_ZIndex = zIndex;
        m_State = State::ACTIVE;

        // Sync position to base shape
        m_BaseShape->Play(position, zIndex);

        // Reset movement modifier and set start position
        if (m_MovementModifier.IsMoving()) {
            m_MovementModifier.Reset();
            m_MovementModifier.SetStartPosition(position);
        }
    }

    void CompositeEffect::Reset() {
        if (m_BaseShape) {
            m_BaseShape->Reset();
        }

        m_ElapsedTime = 0.0f;
        m_State = State::INACTIVE;
    }

} // namespace Effect