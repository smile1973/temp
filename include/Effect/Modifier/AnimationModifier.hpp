#ifndef ANIMATIONMODIFIER_HPP
#define ANIMATIONMODIFIER_HPP

#include "Effect/Shape/BaseShape.hpp"

namespace Effect {
    namespace Modifier {

        // 動畫效果類型枚舉
        enum class AnimationType {
            NONE,       // 無特殊動畫
            RIPPLE,     // 波紋效果
            TRAIL       // 尾跡效果
        };

        class AnimationModifier {
        public:
            AnimationModifier(AnimationType type = AnimationType::NONE, float intensity = 1.0f, float speed = 1.0f);

            // 應用動畫效果
            void Apply(Core::Program& program, float time);

            // 設置參數
            void SetAnimationType(AnimationType type) { m_AnimationType = type; }
            AnimationType GetAnimationType() const { return m_AnimationType; }

            void SetIntensity(float intensity) { m_Intensity = intensity; }
            float GetIntensity() const { return m_Intensity; }

            void SetSpeed(float speed) { m_Speed = speed; }
            float GetSpeed() const { return m_Speed; }

        private:
            AnimationType m_AnimationType;
            float m_Intensity;        // 效果強度
            float m_Speed;            // 效果速度

            GLint m_AnimTypeLocation = -1;
            GLint m_IntensityLocation = -1;
            GLint m_SpeedLocation = -1;
        };

    } // namespace Modifier
} // namespace Effect

#endif //ANIMATIONMODIFIER_HPP
