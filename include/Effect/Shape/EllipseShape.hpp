#ifndef ELLIPSESHAPE_HPP
#define ELLIPSESHAPE_HPP


#include "BaseShape.hpp"

namespace Effect {
    namespace Shape {

        class EllipseShape : public BaseShape {
        public:
            EllipseShape(const glm::vec2& radii = {0.4f, 0.3f}, float duration = 1.0f);
            ~EllipseShape() override;

            // 實現自Core::Drawable
            void Draw(const Core::Matrices& data) override;
            glm::vec2 GetSize() const override { return m_Size; }

            // 設置橢圓屬性
            void SetRadii(const glm::vec2& radii) { m_Radii = radii; }
            const glm::vec2& GetRadii() const { return m_Radii; }

            void SetSize(const glm::vec2& size) { m_Size = size; }

            static Core::Program* GetProgram() { return s_Program.get(); }
            static Core::VertexArray* GetVertexArray() { return s_VertexArray.get(); }

        protected:
            void InitializeResources() override;

            static std::unique_ptr<Core::Program> s_Program;
            static std::unique_ptr<Core::VertexArray> s_VertexArray;

            std::unique_ptr<Core::UniformBuffer<Core::Matrices>> m_MatricesBuffer;

            GLint m_RadiiLocation = -1;
            GLint m_ColorLocation = -1;
            GLint m_TimeLocation = -1;

            glm::vec2 m_Radii = {0.4f, 0.3f}; // 橢圓的x和y半徑
            glm::vec2 m_Size = {400, 400};    // 效果的大小
        };

    } // namespace Shape
} // namespace Effect

#endif //ELLIPSESHAPE_HPP
