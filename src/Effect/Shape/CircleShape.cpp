#include "Effect/Shape/CircleShape.hpp"
#include "Util/Logger.hpp"
#include "config.hpp"

namespace Effect {
    namespace Shape {

        std::unique_ptr<Core::Program> CircleShape::s_Program = nullptr;
        std::unique_ptr<Core::VertexArray> CircleShape::s_VertexArray = nullptr;

        CircleShape::CircleShape(float radius, float duration)
            : BaseShape(duration), m_Radius(radius) {

            // Initialize OpenGL resources (shaders and vertex arrays)
            if (s_Program == nullptr || s_VertexArray == nullptr) {
                CircleShape::InitializeResources();
            }

            m_MatricesBuffer = std::make_unique<Core::UniformBuffer<Core::Matrices>>(
                *s_Program, "Matrices", 0);

            // Get uniform locations for this instance
            s_Program->Bind();
            m_RadiusLocation = glGetUniformLocation(s_Program->GetId(), "u_Radius");
            m_ColorLocation = glGetUniformLocation(s_Program->GetId(), "u_Color");
            m_TimeLocation = glGetUniformLocation(s_Program->GetId(), "u_Time");

            if (m_RadiusLocation == -1 || m_ColorLocation == -1 || m_TimeLocation == -1) {
                LOG_ERROR("Failed to get uniform locations for CircleShape instance");
            }
        }

        CircleShape::~CircleShape() = default;

        void CircleShape::Draw(const Core::Matrices& data) {
            if (m_State != State::ACTIVE) return;

            // Update matrices
            m_MatricesBuffer->SetData(0, data);

            // Enable blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // 不要在這裡綁定程序，因為 CompositeEffect 已經綁定了
            // 只設置自己特有的 uniform 變數，不要重設已由修飾器設置的變數

            // 設置圓形特有的 uniform
            glUniform1f(m_RadiusLocation, m_Radius);

            // 設置顏色 - 這個可能會與修飾器衝突，所以確保順序正確
            glUniform4f(m_ColorLocation, m_Color.r, m_Color.g, m_Color.b, m_Color.a);

            // 設置時間
            glUniform1f(m_TimeLocation, m_ElapsedTime);

            // 不要進行驗證，因為這可能會干擾修飾器的設置
            // s_Program->Validate();

            // Draw
            s_VertexArray->Bind();
            s_VertexArray->DrawTriangles();
        }

        void CircleShape::InitializeResources() {
            // Initialize shader program
            try {
                s_Program = std::make_unique<Core::Program>(
                    GA_RESOURCE_DIR "/shaders/Circle.vert",
                    GA_RESOURCE_DIR "/shaders/Circle.frag");
                LOG_INFO("Circle shape shaders loaded successfully");
            } catch (const std::exception& e) {
                LOG_ERROR("Failed to load circle shape shaders: {}", e.what());
            }

            // Initialize vertex array
            s_VertexArray = std::make_unique<Core::VertexArray>();

            // Set vertex data
            s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
                std::vector<float>{
                    -0.5f, 0.5f,   // top left
                    -0.5f, -0.5f,  // bottom left
                    0.5f, -0.5f,   // bottom right
                    0.5f, 0.5f     // top right
                },
                2));

            // Set UV coordinates
            s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
                std::vector<float>{
                    0.0f, 0.0f,  // top left
                    0.0f, 1.0f,  // bottom left
                    1.0f, 1.0f,  // bottom right
                    1.0f, 0.0f   // top right
                },
                2));

            // Set indices
            s_VertexArray->SetIndexBuffer(std::make_unique<Core::IndexBuffer>(
                std::vector<unsigned int>{
                    0, 1, 2,  // first triangle
                    0, 2, 3   // second triangle
                }));
        }
    } // namespace Shape
} // namespace Effect