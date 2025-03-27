#include "Enemy.hpp"

// 初始化靜態成員：著色程序和頂點數據
std::unique_ptr<Core::Program> Enemy::s_Program = nullptr;
std::unique_ptr<Core::VertexArray> Enemy::s_VertexArray = nullptr;

// 構造函數，初始化敵人的生命值與繪製屬性
Enemy::Enemy(float health, const std::vector<std::string>& ImageSet)
    : Character(ImageSet), m_Health(health), m_MaxHealth(health),
      m_ColorLocation(-1), m_WidthLocation(-1) {

    // 確保著色程序（Shader Program）只初始化一次
    if (!s_Program) {
        InitProgram();
    }
    // 確保頂點數據（Vertex Array）只初始化一次
    if (!s_VertexArray) {
        InitVertexArray();
    }

    // 初始化 Uniform 變數（著色器中的全域變數）
    InitUniforms();
}

// 讓敵人受到傷害，減少生命值
void Enemy::TakeDamage(float damage) {
    if (this->GetVisibility()) {
        m_Health = std::max(0.0f, m_Health - damage);
        LOG_DEBUG("Enemy took {:.1f} damage, remaining health: {:.1f}", damage, m_Health);

        if (! this->IfAlive()) {
            this->SetVisible(false);
            LOG_DEBUG("The Enemy dies");
        }
    }
}

void Enemy::SetHealth(const float Health) {
    if (Health == -1.0f) {
        m_Health = m_MaxHealth;
    } else {
        m_Health = Health;
        m_MaxHealth = Health;
    }
}

// 繪製敵人的血條
void Enemy::DrawHealthBar(const glm::vec2& position) const {
    if (!s_Program || !s_VertexArray) return;

    // 啟用透明度混合，以確保血條能夠正確顯示
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    s_Program->Bind();

    // 設定血條顏色為紅色
    glUniform4f(m_ColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);

    // 根據當前生命值調整血條寬度
    float currentWidth = m_Health / m_MaxHealth; // 0 ~ 1 縮放
    glUniform1f(glGetUniformLocation(s_Program->GetId(), "u_Width"), currentWidth);
    glUniform2f(glGetUniformLocation(s_Program->GetId(), "u_Position"), position.x, position.y);


    s_Program->Validate(); // 確保著色程序運行正常

    // 綁定並繪製血條
    s_VertexArray->Bind();
    s_VertexArray->DrawTriangles();
    s_Program->Unbind();
}

// 初始化著色程序，為血條載入對應的著色器文件
void Enemy::InitProgram() {
    try {
        s_Program = std::make_unique<Core::Program>(
            GA_RESOURCE_DIR "/shaders/HealthBar.vert",
            GA_RESOURCE_DIR "/shaders/HealthBar.frag"
        );
        LOG_INFO("Health bar shaders loaded successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load health bar shaders: {}", e.what());
        s_Program.reset();
    }
}

// 初始化血條的頂點數據
void Enemy::InitVertexArray() {
    s_VertexArray = std::make_unique<Core::VertexArray>();

    // 定義血條矩形的四個頂點（確保右對齊縮放）
    float Width = 1.8f, halfHeight = 0.01f;
    s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(std::vector<float>{
        -Width,  halfHeight,  // 左上
        -Width, -halfHeight,  // 左下
         0.0, -halfHeight,  // 右下
         0.0,  halfHeight   // 右上
    }, 2));

    // UV 坐標對應頂點
    s_VertexArray->AddVertexBuffer(std::make_unique<Core::VertexBuffer>(std::vector<float>{
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    }, 2));

    // 定義繪製順序（索引緩衝）
    s_VertexArray->SetIndexBuffer(std::make_unique<Core::IndexBuffer>(std::vector<unsigned int>{
        0, 1, 2,  // 第一個三角形
        0, 2, 3   // 第二個三角形
    }));
}

// 獲取 Shader Program 中的 Uniform 變數位置
void Enemy::InitUniforms() {
    if (!s_Program) return;

    s_Program->Bind();
    m_ColorLocation = glGetUniformLocation(s_Program->GetId(), "u_Color");
    m_WidthLocation = glGetUniformLocation(s_Program->GetId(), "u_Width");

    // 檢查 Uniform 變數是否成功獲取
    if (m_ColorLocation == -1 || m_WidthLocation == -1) {
        LOG_ERROR("Failed to get uniform locations for HealthBar");
    }
}
