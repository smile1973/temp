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
    // auto pattern1 = AttackPatternFactory::GetInstance().CreateBattle2Pattern();

    // 中央位置
    glm::vec2 centerPosition(0.0f, 0.0f);

    // 移動敵人到中心
    auto moveToCenter = [centerPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(centerPosition);
        LOG_DEBUG("Enemy moved to center for Battle 3");
    };

    // 創建第一個模式：十字旋轉雷射
    // 直接使用已有的工廠方法
    auto pattern2 = AttackPatternFactory::GetInstance().CreateCrossRotatingLaserPattern(
        centerPosition,   // 中心位置
        1500.0f,           // 寬度
        60.0f,         // 高度 -- 寬高以0度為準
        0.2f,            // 旋轉速度
        16.0f,            // 持續時間
        3.0f             // 倒數時間
    );

    // 敵人移動到中心
    // pattern2->AddEnemyMovement(moveToCenter, 0.0f);

    // // 第二個模式：移動的圓形攻擊陣列
    // auto pattern3 = std::make_shared<AttackPattern>();
    //
    // // 創建從左到右的移動圓形攻擊
    // for (int i = 0; i < 5; i++) {
    //     glm::vec2 startPos(-800.0f, -200.0f + i * 100.0f);
    //     glm::vec2 endPos(800.0f, -200.0f + i * 100.0f);
    //
    //     auto attack = std::make_shared<CircleAttack>(startPos, 2.0f, 80.0f, i + 1);
    //     attack->SetColor(Util::Color::FromRGB(255, 100, 0, 200));
    //     attack->SetMovementParams(glm::normalize(endPos - startPos), 350.0f, glm::length(endPos - startPos));
    //
    //     pattern3->AddAttack(attack, i * 0.5f);
    // }
    //
    // pattern3->SetDuration(10.0f);
    //
    // // 敵人移動到中心上方
    // pattern3->AddEnemyMovement([](std::shared_ptr<Enemy> enemy) {
    //     enemy->SetPosition({0.0f, 200.0f});
    //     LOG_DEBUG("Enemy moved to top for Battle 3 pattern 2");
    // }, 0.0f);

    // 添加到隊列
    // AddPattern(pattern1);
    AddPattern(pattern2);
    // AddPattern(pattern3);

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