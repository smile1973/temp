#include "Effect/Modifier/AnimationModifier.hpp"
#include "Util/Logger.hpp"

namespace Effect {
    namespace Modifier {

        AnimationModifier::AnimationModifier(AnimationType type, float intensity, float speed)
            : m_AnimationType(type), m_Intensity(intensity), m_Speed(speed) {
        }

        void AnimationModifier::Apply(Core::Program& program, float time) {
            (void) time;
            // 如果是第一次應用，獲取uniform位置
            if (m_AnimTypeLocation == -1) {
                m_AnimTypeLocation = glGetUniformLocation(program.GetId(), "u_AnimType");
                m_IntensityLocation = glGetUniformLocation(program.GetId(), "u_Intensity");
                m_SpeedLocation = glGetUniformLocation(program.GetId(), "u_AnimSpeed");

                if (m_AnimTypeLocation == -1 || m_IntensityLocation == -1 || m_SpeedLocation == -1) {
                    LOG_ERROR("Failed to get uniform locations for AnimationModifier");
                }
            }

            // 設置uniform值
            glUniform1i(m_AnimTypeLocation, static_cast<int>(m_AnimationType));
            glUniform1f(m_IntensityLocation, m_Intensity);
            glUniform1f(m_SpeedLocation, m_Speed);
        }

    } // namespace Modifier
} // namespace Effect