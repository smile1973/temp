#include "Effect/Modifier/EdgeModifier.hpp"
#include "Util/Logger.hpp"

namespace Effect {
    namespace Modifier {

        EdgeModifier::EdgeModifier(EdgeType type, float width, const Util::Color& edgeColor)
            : m_EdgeType(type), m_Width(width), m_EdgeColor(edgeColor) {
        }

        // 修改 EdgeModifier.cpp 中的 Apply 方法
        void EdgeModifier::Apply(Core::Program& program) {
            // 每次應用時都重新獲取 uniform 位置，確保適用於當前綁定的程序
            m_EdgeTypeLocation = glGetUniformLocation(program.GetId(), "u_EdgeType");
            m_EdgeWidthLocation = glGetUniformLocation(program.GetId(), "u_EdgeWidth");
            m_EdgeColorLocation = glGetUniformLocation(program.GetId(), "u_EdgeColor");

            // 添加更詳細的日誌，顯示為哪個程序設置 uniform
            // LOG_DEBUG("EdgeModifier applying to program {}: edgeType={}, edgeWidth={}, edgeColor={}",
            //          program.GetId(), m_EdgeTypeLocation, m_EdgeWidthLocation, m_EdgeColorLocation);

            if (m_EdgeTypeLocation == -1 || m_EdgeWidthLocation == -1 || m_EdgeColorLocation == -1) {
                LOG_ERROR("Failed to get uniform locations for EdgeModifier in program {}", program.GetId());

                // 檢查程序中的活躍 uniform
                GLint numUniforms = 0;
                glGetProgramiv(program.GetId(), GL_ACTIVE_UNIFORMS, &numUniforms);
                LOG_DEBUG("Program {} has {} active uniforms", program.GetId(), numUniforms);

                for (GLint i = 0; i < numUniforms; i++) {
                    char name[128];
                    GLint size;
                    GLenum type;
                    glGetActiveUniform(program.GetId(), i, sizeof(name), nullptr, &size, &type, name);
                    LOG_DEBUG("  Uniform {}: name={}, size={}, type={}", i, name, size, type);
                }

                return;
            }

            // 設置 uniform 值 - 即使獲取失敗也嘗試設置，這可能會導致 GL 錯誤，但不會導致崩潰
            glUniform1i(m_EdgeTypeLocation, static_cast<int>(m_EdgeType));
            glUniform1f(m_EdgeWidthLocation, m_Width);
            glUniform4f(m_EdgeColorLocation, m_EdgeColor.r, m_EdgeColor.g, m_EdgeColor.b, m_EdgeColor.a);
        }

    } // namespace Modifier
} // namespace Effect
