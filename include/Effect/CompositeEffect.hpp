#ifndef COMPOSITEEFFECT_HPP
#define COMPOSITEEFFECT_HPP

#include "Effect/Shape/CircleShape.hpp"
#include "Effect/Shape/EllipseShape.hpp"
#include "Effect/Shape/RectangleShape.hpp"
#include "Effect/Modifier/FillModifier.hpp"
#include "Effect/Modifier/EdgeModifier.hpp"
#include "Effect/Modifier/MovementModifier.hpp"
#include "Effect/Modifier/AnimationModifier.hpp"

namespace Effect {

    // 組合所有修飾器的特效類
    class CompositeEffect : public IEffect {
    public:
        // 建構函數基於形狀類型
        CompositeEffect(const std::shared_ptr<Shape::BaseShape>& baseShape);
        ~CompositeEffect() override = default;

        // 實現自Core::Drawable
        void Draw(const Core::Matrices& data) override;
        glm::vec2 GetSize() const override;

        // 實現自Effect::IEffect
        void Update(float deltaTime) override;
        void Play(const glm::vec2& position, float zIndex = 0.0f) override;
        void Reset() override;

        // 修飾器設置
        void SetFillModifier(const Modifier::FillModifier& modifier) { m_FillModifier = modifier; }
        void SetEdgeModifier(const Modifier::EdgeModifier& modifier) { m_EdgeModifier = modifier; }
        void SetMovementModifier(const Modifier::MovementModifier& modifier) { m_MovementModifier = modifier; }
        void SetAnimationModifier(const Modifier::AnimationModifier& modifier) { m_AnimationModifier = modifier; }

        // 基礎形狀設置
        std::shared_ptr<Shape::BaseShape> GetBaseShape() { return m_BaseShape; }

    private:
        std::shared_ptr<Shape::BaseShape> m_BaseShape;
        Modifier::FillModifier m_FillModifier;
        Modifier::EdgeModifier m_EdgeModifier;
        Modifier::MovementModifier m_MovementModifier;
        Modifier::AnimationModifier m_AnimationModifier;
    };

} // namespace Effect

#endif //COMPOSITEEFFECT_HPP
