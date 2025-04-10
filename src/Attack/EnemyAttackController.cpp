#include "Attack/EnemyAttackController.hpp"
#include "Attack/AttackManager.hpp"
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

    // 中央位置
    glm::vec2 centerPosition(0.0f, 0.0f);

    // 移動敵人到中心
    auto moveToCenter = [centerPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(centerPosition);
        LOG_DEBUG("Enemy moved to center for Battle 2");
    };

    auto pattern2 = AttackPatternFactory::GetInstance().CreateCrossRotatingLaserPattern(
        centerPosition,   // 中心位置
        1500.0f,          // 寬度
        100.0f,          // 高度
        0.3f,            // 旋轉速度
        4.0f,            // 持續時間
        1.5f             // 倒數時間
    );
    pattern2->AddEnemyMovement(moveToCenter, 0.0f);

    auto attack = std::make_shared<CornerBulletAttack>(2.0, 3);
    attack->SetBulletSpeed(1000);
    attack->SetBulletRadius(35.f);
    pattern2->AddAttack(attack, 4.0f);
    pattern2->SetDuration(7.0f);

    auto pattern = AttackPatternFactory::GetInstance().CreateCornerBulletPattern(
        3,
        600.0f, // 子彈速度
        30.0f,  // 子彈半徑
        1.5f    // 倒數時間
    );

    glm::vec2 startPos(640.0f, -200.0f);
    glm::vec2 endPos(-640.0f, -200.0f);
    auto attacka = std::make_shared<CircleAttack>(startPos, 2.0f, 180.0f);
    attacka->SetColor(Util::Color(1.0, 0.4, 0.4, 0.5));
    attacka->SetMovementParams(glm::normalize(endPos - startPos), 100.0f, glm::length(endPos - startPos));
    pattern->AddAttack(attacka, 0);

    auto pattern4 = AttackPatternFactory::GetInstance().CreateCrossRotatingLaserPattern(
        centerPosition,   // 中心位置
        1500.0f,          // 寬度
        100.0f,          // 高度
        -0.2f,           // 旋轉速度
        4.0f,            // 持續時間
        3.0f             // 倒數時間
    );

    for (int i = 0; i < 3; i++) {
        startPos = glm::vec2(640.0f, -300.0f + i * 300.0f);
        endPos = glm::vec2(-640.0f, -300.0f + i * 300.0f);

        auto attack = std::make_shared<CircleAttack>(startPos, 2.0f, 120.0f, i + 1);
        attack->SetColor(Util::Color(1.0, 0.4, 0.4, 0.5));
        attack->SetMovementParams(glm::normalize(endPos - startPos), 450.0f, glm::length(endPos - startPos));

        pattern4->AddAttack(attack, 0);
    }

    for (int i = 0; i < 8; i++) {
        startPos = glm::vec2(-600.0f + i * 180.0f, 360.0f);
        endPos = glm::vec2(-600.0f + i * 180.0f, -360.0f);

        auto attack = std::make_shared<CircleAttack>(startPos, 2.0f, 80.0f, i + 1);
        attack->SetColor(Util::Color(1.0, 0.4, 0.4, 0.5));
        attack->SetMovementParams(glm::normalize(endPos - startPos), 300.0f, glm::length(endPos - startPos));

        pattern4->AddAttack(attack, 2);
    }

    // 添加到隊列
    AddPattern(pattern2);
    AddPattern(pattern);
    AddPattern(pattern4);

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
}

void EnemyAttackController::Reset() {
    Stop();
    ClearPatterns();
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