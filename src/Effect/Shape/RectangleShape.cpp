#include "Effect/Shape/RectangleShape.hpp"
#include "Util/Logger.hpp"
#include "config.hpp"

namespace Effect {
    namespace Shape {

        std::unique_ptr<Core::Program> RectangleShape::s_Program = nullptr;
        std::unique_ptr<Core::VertexArray> RectangleShape::s_VertexArray = nullptr;

        RectangleShape::RectangleShape(const glm::vec2& dimensions, float thickness, float rotation, float duration, bool autoRotate, float rotationSpeed)
            : BaseShape(duration), m_Dimensions(dimensions), m_Thickness(thickness), m_Rotation(rotation),
              m_AutoRotate(autoRotate), m_RotationSpeed(rotationSpeed) {

            // Initialize OpenGL resources
            if (s_Program == nullptr || s_VertexArray == nullptr) {
                RectangleShape::InitializeResources();
            }

            m_MatricesBuffer = std::make_unique<Core::UniformBuffer<Core::Matrices>>(
                *s_Program, "Matrices", 0);

            // Get uniform locations for this instance
            s_Program->Bind();
            m_DimensionsLocation = glGetUniformLocation(s_Program->GetId(), "u_Dimensions");
            m_ThicknessLocation = glGetUniformLocation(s_Program->GetId(), "u_Thickness");
            m_RotationLocation = glGetUniformLocation(s_Program->GetId(), "u_Rotation");
            m_ColorLocation = glGetUniformLocation(s_Program->GetId(), "u_Color");
            m_TimeLocation = glGetUniformLocation(s_Program->GetId(), "u_Time");

            // 確保所有 uniform 變數都能被找到
            if (m_DimensionsLocation == -1 || m_ThicknessLocation == -1 ||
                m_RotationLocation == -1 || m_ColorLocation == -1 ||
                m_TimeLocation == -1) {
                LOG_ERROR("Failed to get basic uniform locations for RectangleShape instance");
            }

            // 檢查填充、邊緣和動畫修飾器相關的 uniform 變數也存在
            GLint fillTypeLocation = glGetUniformLocation(s_Program->GetId(), "u_FillType");
            GLint fillThicknessLocation = glGetUniformLocation(s_Program->GetId(), "u_FillThickness");
            GLint edgeTypeLocation = glGetUniformLocation(s_Program->GetId(), "u_EdgeType");
            GLint edgeWidthLocation = glGetUniformLocation(s_Program->GetId(), "u_EdgeWidth");
            GLint edgeColorLocation = glGetUniformLocation(s_Program->GetId(), "u_EdgeColor");
            GLint animTypeLocation = glGetUniformLocation(s_Program->GetId(), "u_AnimType");
            GLint intensityLocation = glGetUniformLocation(s_Program->GetId(), "u_Intensity");
            GLint speedLocation = glGetUniformLocation(s_Program->GetId(), "u_AnimSpeed");

            if (fillTypeLocation == -1 || fillThicknessLocation == -1) {
                LOG_ERROR("Failed to get uniform locations for FillModifier in RectangleShape");
            }
            if (edgeTypeLocation == -1 || edgeWidthLocation == -1 || edgeColorLocation == -1) {
                LOG_ERROR("Failed to get uniform locations for EdgeModifier in RectangleShape");
            }
            if (animTypeLocation == -1 || intensityLocation == -1 || speedLocation == -1) {
                LOG_ERROR("Failed to get uniform locations for AnimationModifier in RectangleShape");
            }
        }

        RectangleShape::~RectangleShape() = default;

        void RectangleShape::Draw(const Core::Matrices& data) {
            if (m_State != State::ACTIVE) return;

            // Update matrices
            m_MatricesBuffer->SetData(0, data);

            // Enable blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Bind shader program
            s_Program->Bind();

            // Set uniforms with instance values
            glUniform2f(m_DimensionsLocation, m_Dimensions.x, m_Dimensions.y);
            glUniform1f(m_ThicknessLocation, m_Thickness); // 保持厚度的設置
            glUniform1f(m_RotationLocation, m_Rotation);

            // LOG_DEBUG("RectangleShape::Draw - Rotation: {:.2f} radians", m_Rotation);

            // Set color properly
            glUniform4f(m_ColorLocation, m_Color.r, m_Color.g, m_Color.b, m_Color.a);

            // Set time for animation
            glUniform1f(m_TimeLocation, m_ElapsedTime);

            // 重要：不要在這裡設置修飾器的默認值，因為這可能會覆蓋 CompositeEffect 中的設置
            // 只有在確定修飾器未設置時才應用默認值 (讓修飾器自己來設置)

            // Validate shader program (檢查著色器是否一切正常)
            s_Program->Validate();

            // Draw
            s_VertexArray->Bind();
            s_VertexArray->DrawTriangles();
        }
        void RectangleShape::Update(float deltaTime) {
            // 調用基類更新
            BaseShape::Update(deltaTime);

            // 僅在啟用自動旋轉且特效處於活躍狀態時更新旋轉角度
            if (m_AutoRotate && m_State == State::ACTIVE) {
                // 根據旋轉速度更新角度
                m_Rotation += deltaTime * m_RotationSpeed * 3.14159f;

                // 保持旋轉角度在0-2π範圍內
                while (m_Rotation > 2.0f * 3.14159f) {
                    m_Rotation -= 2.0f * 3.14159f;
                }

                // 每1.5秒記錄一次旋轉角度（避免日誌過多）
                static float logTimer = 0.0f;
                logTimer += deltaTime;
                if (logTimer > 1.5f) {
                    LOG_DEBUG("RectangleShape auto-rotating: {:.2f} radians, speed: {:.2f}",
                              m_Rotation, m_RotationSpeed);
                    logTimer = 0.0f;
                }
            }
        }

        void RectangleShape::InitializeResources() {
            // Initialize shader program
            try {
                s_Program = std::make_unique<Core::Program>(
                    GA_RESOURCE_DIR "/shaders/Rectangle.vert",
                    GA_RESOURCE_DIR "/shaders/Rectangle.frag");
                LOG_INFO("Rectangle shape shaders loaded successfully");
            } catch (const std::exception& e) {
                LOG_ERROR("Failed to load rectangle shape shaders: {}", e.what());
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