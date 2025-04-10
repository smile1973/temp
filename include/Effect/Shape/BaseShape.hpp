#ifndef BASESHAPE_HPP
#define BASESHAPE_HPP

#include "Effect/IEffect.hpp"
#include "Core/Program.hpp"
#include "Core/VertexArray.hpp"
#include "Core/UniformBuffer.hpp"
#include "Util/Color.hpp"

namespace Effect {
    namespace Shape {

        class BaseShape : public IEffect {
        public:
            BaseShape(float duration = 1.0f);
            virtual ~BaseShape() = default;

            // Methods inherited from IEffect
            void Draw(const Core::Matrices& data) override = 0;
            void Update(float deltaTime) override;
            void Play(const glm::vec2& position, float zIndex = 0.0f) override;
            void Reset() override;

            // Set basic properties
            void SetColor(const Util::Color& color) { m_Color = color; }
            const Util::Color& GetColor() const { return m_Color; }

            // User data for tracking (used to store effect type)
            void SetUserData(int data) { m_UserData = data; }
            int GetUserData() const { return m_UserData; }

        protected:
            Util::Color m_Color = Util::Color::FromName(Util::Colors::PINK);
            virtual void InitializeResources() = 0; // Subclasses must implement this method to initialize OpenGL resources

        private:
            int m_UserData = -1; // Used to store the effect type
        };
    } // namespace Shape
} // namespace Effect

#endif //BASESHAPE_HPP