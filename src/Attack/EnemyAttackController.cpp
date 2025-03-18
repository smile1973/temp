// src/Attack/EnemyAttackController.cpp
#include "Attack/EnemyAttackController.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"

EnemyAttackController::EnemyAttackController(std::shared_ptr<Enemy> enemy)
    : m_Enemy(enemy) {
}

void EnemyAttackController::InitBattle1Patterns() {
    // 清空現有的模式
    ClearPatterns();

    // 獲取Battle 1的預定義攻擊模式
    auto pattern1 = AttackPatternFactory::GetInstance().CreateBattle1Pattern();

    // 再添加一個簡單的環形攻擊模式作為第二個模式
    glm::vec2 centerPosition(0.0f, 0.0f);
    auto pattern2 = AttackPatternFactory::GetInstance().CreateCircularPattern(
        centerPosition, 200.0f, 80.0f, 6, 2.0f, 0.4f
    );

    // 添加到隊列
    AddPattern(pattern1);
    AddPattern(pattern2);

    LOG_DEBUG("Battle 1 attack patterns initialized");
}

void EnemyAttackController::InitBattle2Patterns() {
    // 清空現有的模式
    ClearPatterns();

    // 獲取Battle 2的預定義攻擊模式
    auto pattern1 = AttackPatternFactory::GetInstance().CreateBattle2Pattern();

    // 添加一個矩形攻擊模式作為第二個模式
    glm::vec2 centerPosition(0.0f, 0.0f);

    // 創建多個旋轉的矩形攻擊
    auto pattern2 = std::make_shared<AttackPattern>();

    // 敵人移動到中心
    pattern2->AddEnemyMovement([centerPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(centerPosition);
    }, 0.0f);

    // 添加4個旋轉的矩形攻擊
    float rotations[] = {0.0f, 0.7854f, 1.5708f, 2.3562f}; // 0, 45, 90, 135度
    float startTime = 1.0f;

    for (int i = 0; i < 4; ++i) {
        auto attack = std::make_shared<RectangleAttack>(
            centerPosition, 2.5f, 500.0f, 80.0f, rotations[i], i + 1
        );
        pattern2->AddAttack(attack, startTime);
        startTime += 1.0f;
    }

    pattern2->SetDuration(8.0f);

    // 添加到隊列
    AddPattern(pattern1);
    AddPattern(pattern2);

    LOG_DEBUG("Battle 2 attack patterns initialized");
}

void EnemyAttackController::Update(float deltaTime, std::shared_ptr<Character> player) {
    if (!m_IsActive) return;

    // 處理冷卻
    if (m_IsInCooldown) {
        m_ElapsedCooldownTime += deltaTime;

        if (m_ElapsedCooldownTime >= m_CooldownTime) {
            // 冷卻結束，切換到下一個模式
            m_IsInCooldown = false;
            m_ElapsedCooldownTime = 0.0f;
            SwitchToNextPattern();
        }
        return;
    }

    // 更新當前攻擊模式
    if (m_CurrentPattern) {
        m_CurrentPattern->Update(deltaTime, player);

        // 檢查當前模式是否已完成
        if (m_CurrentPattern->IsFinished()) {
            LOG_DEBUG("Attack pattern completed");

            // 開始冷卻
            m_IsInCooldown = true;
            m_ElapsedCooldownTime = 0.0f;
        }
    } else {
        // 沒有當前模式，檢查隊列
        SwitchToNextPattern();
    }
}

bool EnemyAttackController::IsAllPatternsCompleted() const {
    return !m_CurrentPattern && m_PatternQueue.empty() && !m_IsInCooldown;
}

void EnemyAttackController::AddPattern(std::shared_ptr<AttackPattern> pattern) {
    if (pattern) {
        m_PatternQueue.push(pattern);
    }
}

void EnemyAttackController::ClearPatterns() {
    while (!m_PatternQueue.empty()) {
        m_PatternQueue.pop();
    }

    m_CurrentPattern = nullptr;
    m_IsInCooldown = false;
    m_ElapsedCooldownTime = 0.0f;
}

void EnemyAttackController::Start() {
    m_IsActive = true;

    // 如果有當前模式，直接啟動
    if (m_CurrentPattern) {
        m_CurrentPattern->Start(m_Enemy);
    } else {
        // 嘗試切換到下一個模式
        SwitchToNextPattern();
    }

    LOG_DEBUG("Enemy attack controller started");
}

void EnemyAttackController::Stop() {
    m_IsActive = false;

    // 停止當前模式
    if (m_CurrentPattern) {
        m_CurrentPattern->Stop();
    }

    LOG_DEBUG("Enemy attack controller stopped");
}

void EnemyAttackController::Reset() {
    Stop();
    ClearPatterns();
    LOG_DEBUG("Enemy attack controller reset");
}

void EnemyAttackController::SwitchToNextPattern() {
    // 如果隊列為空，沒有更多模式
    if (m_PatternQueue.empty()) {
        m_CurrentPattern = nullptr;
        return;
    }

    // 獲取下一個模式
    m_CurrentPattern = m_PatternQueue.front();
    m_PatternQueue.pop();

    // 開始新模式
    if (m_CurrentPattern && m_IsActive) {
        m_CurrentPattern->Start(m_Enemy);
        LOG_DEBUG("Switched to next attack pattern");
    }
}