#include "Enemy.hpp"
#include "Attack/AttackManager.hpp"
#include "App.hpp"
// // 初始化靜態成員：著色程序和頂點數據
// std::unique_ptr<Core::Program> Enemy::s_Program = nullptr;
// std::unique_ptr<Core::VertexArray> Enemy::s_VertexArray = nullptr;

// 構造函數，初始化敵人的生命值與繪製屬性
Enemy::Enemy(std::string name, const float health, const std::vector<std::string>& ImageSet)
    : Character(ImageSet), m_Name(std::move(name)), m_Health(health), m_MaxHealth(health),
      m_ColorLocation(-1), m_WidthLocation(-1) {

    m_Transform.scale = {0.5f, 0.5f};
    SetZIndex(10);
    SetVisible(false);

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
    InitHealthRing();
}

// 讓敵人受到傷害，減少生命值
void Enemy::TakeDamage(float damage) {
    if (! this->GetVisibility()) return;
    m_Health = std::max(0.0f, m_Health - damage);
    LOG_DEBUG("{} took {:.1f} damage, remaining health: {:.1f}", m_Name, damage, m_Health);

    UpdateHealthRing();
    if (! this->IfAlive()) {
        this->SetVisible(false);
        LOG_DEBUG("The Enemy {} dies", m_Name);
        if (! this->IfAlive()) {
            this->SetVisible(false);
            Effect::EffectManager::GetInstance().ClearAllEffects();
            AttackManager::GetInstance().ClearAllAttacks(); // 清除所有攻擊
            App::GetInstance().GetOverlay()->SetVisible(false);
            LOG_DEBUG("The Enemy dies");
        }
    }
}

void Enemy::SetHealth(const float Health) {
    m_Health = (Health == -1.0f) ? m_MaxHealth : Health;
    m_MaxHealth = (Health == -1.0f) ? m_MaxHealth : Health;
}

void Enemy::MovePosition(const glm::vec2& Position, const float totalTime) {
    MoveToPosition(GetPosition() + Position, totalTime);
}

void Enemy::MoveToPosition(const glm::vec2& targetPosition, const float totalTime) {
    if (totalTime <= 0.0f) {
        m_Transform.translation = targetPosition;
        return;
    }

    m_IsMoving = true;
    m_TargetPosition = targetPosition;
    m_MaxDistance = glm::distance(this->GetPosition(), targetPosition);
    m_Direction = (targetPosition - this->GetPosition()) / m_MaxDistance;
    m_Speed = m_MaxDistance / totalTime;
    m_TotalTime = totalTime * 1000.0f; //(ms)
    LOG_DEBUG("Move {} to {}",m_Name , m_Transform.translation);
}

void Enemy::Update() {
    // if (!m_IsMoving || !GetVisibility()) return;
    if (!m_IsMoving) return;
    // 計算移動距離
    const float DeltaTimeMs = Util::Time::GetDeltaTimeMs();
    const float moveDistance = m_Speed * DeltaTimeMs / 1000.0f;
    m_DistanceTraveled += moveDistance;
    // 更新位置
    m_Transform.translation += m_Direction * moveDistance;
    // 檢查是否到達目標距離
    if (m_DistanceTraveled >= m_MaxDistance) {
        m_IsMoving = false; // 停止移動
        m_Transform.translation = m_TargetPosition;
        m_DistanceTraveled = 0;
        LOG_DEBUG("{} move to {}",m_Name , m_Transform.translation);
    }
    UpdateHealthRing();
}

void Enemy::InitHealthRing() {
    // 創建半透明背景圈
    m_HealthRingBackground = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(GA_RESOURCE_DIR "/Image/Hitbox.png"),
        m_ZIndex - 1  // 確保在敵人下方
    );

    // 設置合適的大小
    m_HealthRingBackground->m_Transform.scale = glm::vec2(1.5f, 1.5f);
    m_HealthRingBackground->SetVisible(false);  // 初始隱藏

    // 將背景圈添加到渲染樹
    App::GetInstance().AddToRoot(m_HealthRingBackground);

    // 創建點狀血條
    m_HealthDots.clear();
    for (int i = 0; i < m_TotalDots; i++) {
        auto dot = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(GA_RESOURCE_DIR "/Image/healthPoint.png"),
            m_ZIndex + 1  // 確保在敵人上方
        );

        // 設置點的大小
        dot->m_Transform.scale = glm::vec2(0.3f, 0.3f);
        dot->SetVisible(false);  // 初始隱藏

        // 計算點的位置 (圓形分布)
        float angle = 2.0f * M_PI * i / m_TotalDots;
        float x = cos(angle) * m_RingRadius;
        float y = sin(angle) * m_RingRadius;

        dot->m_Transform.translation = glm::vec2(x, y);

        m_HealthDots.push_back(dot);
        App::GetInstance().AddToRoot(dot);
    }
}

void Enemy::UpdateHealthRing() {
    if (!this->GetVisibility()) {
        // 敵人不可見時隱藏血條環
        if (m_HealthRingBackground) {
            m_HealthRingBackground->SetVisible(false);
        }

        for (auto& dot : m_HealthDots) {
            if (dot) {
                dot->SetVisible(false);
            }
        }
        return;
    }

    // 敵人可見時顯示並更新血條環

    // 更新背景圈位置
    if (m_HealthRingBackground) {
        m_HealthRingBackground->m_Transform.translation = this->GetPosition();
        m_HealthRingBackground->SetVisible(true);
    }

    // 計算應該顯示的點數量
    float healthPercent = static_cast<float>(m_Health) / static_cast<float>(m_MaxHealth);
    int visibleDots = static_cast<int>(healthPercent * m_TotalDots);

    // 更新每個點的位置和可見性
    for (int i = 0; i < m_TotalDots; i++) {
        if (i < m_HealthDots.size() && m_HealthDots[i]) {
            // 計算點的位置 (相對於敵人)
            float angle = 2.0f * M_PI * i / m_TotalDots;
            float x = cos(angle) * m_RingRadius;
            float y = sin(angle) * m_RingRadius;

            m_HealthDots[i]->m_Transform.translation = this->GetPosition() + glm::vec2(x, y);

            // 根據血量決定點的可見性
            m_HealthDots[i]->SetVisible(i < visibleDots);

            // 可以設置不同的顏色 (例如前半部分紅色，後半部分黃色)
            // 如果您有這樣的需求，可以添加相關代碼
        }
    }
}
