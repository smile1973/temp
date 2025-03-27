#include "Effect/Modifier/FillModifier.hpp"
#include "Util/Logger.hpp"

namespace Effect {
    namespace Modifier {

        FillModifier::FillModifier(FillType type, float thickness)
            : m_FillType(type), m_Thickness(thickness) {
        }

        void FillModifier::Apply(Core::Program& program) {
            // 如果是第一次應用，獲取uniform位置
            if (m_FillTypeLocation == -1) {
                m_FillTypeLocation = glGetUniformLocation(program.GetId(), "u_FillType");
                m_ThicknessLocation = glGetUniformLocation(program.GetId(), "u_Thickness");

                // 如果找不到標準位置，嘗試查找替代名稱 (用於 Rectangle 著色器)
                if (m_ThicknessLocation == -1) {
                    m_ThicknessLocation = glGetUniformLocation(program.GetId(), "u_FillThickness");
                }

                if (m_FillTypeLocation == -1 || m_ThicknessLocation == -1) {
                    LOG_ERROR("Failed to get uniform locations for FillModifier");
                }
            }

            // 設置uniform值
            glUniform1i(m_FillTypeLocation, static_cast<int>(m_FillType));
            glUniform1f(m_ThicknessLocation, m_Thickness);

            // 額外嘗試設置特定於 Rectangle 著色器的 uniform 變數
            GLint fillThicknessLocation = glGetUniformLocation(program.GetId(), "u_FillThickness");
            if (fillThicknessLocation != -1 && fillThicknessLocation != m_ThicknessLocation) {
                glUniform1f(fillThicknessLocation, m_Thickness);
            }
        }
    } // namespace Modifier
} // namespace Effect