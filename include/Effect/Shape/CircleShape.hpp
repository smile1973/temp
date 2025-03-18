#ifndef CIRCLESHAPE_HPP
#define CIRCLESHAPE_HPP
#include "BaseShape.hpp"

namespace Effect {
    namespace Shape {

        class CircleShape : public BaseShape {
        public:
            CircleShape(float radius = 0.4f, float duration = 1.0f);
            ~CircleShape() override;

            // 實現自Core::Drawable
            void Draw(const Core::Matrices& data) override;
            glm::vec2 GetSize() const override { return m_Size; }

            // 設置圓形屬性
            void SetRadius(float radius) { m_Radius = radius; }
            float GetRadius() const { return m_Radius; }

            void SetSize(const glm::vec2& size) { m_Size = size; }

            static Core::Program* GetProgram() { return s_Program.get(); }
            static Core::VertexArray* GetVertexArray() { return s_VertexArray.get(); }

        protected:
            void InitializeResources() override;

            static std::unique_ptr<Core::Program> s_Program;
            static std::unique_ptr<Core::VertexArray> s_VertexArray;

            std::unique_ptr<Core::UniformBuffer<Core::Matrices>> m_MatricesBuffer;

            GLint m_RadiusLocation = -1;
            GLint m_ColorLocation = -1;
            GLint m_TimeLocation = -1;

            float m_Radius = 0.4f;           // 圓的半徑
            glm::vec2 m_Size = {400, 400};   // 效果的大小
        };

    } // namespace Shape
} // namespace Effect

#endif //CIRCLESHAPE_HPP
