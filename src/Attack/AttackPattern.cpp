// src/AttackPattern.cpp
#include "Attack/AttackPattern.hpp"
#include "Util/Logger.hpp"
#include <algorithm>

AttackPattern::AttackPattern() {}

void AttackPattern::AddAttack(std::shared_ptr<Attack> attack, float startTime) {
    // 新增攻擊到列表中
    m_Attacks.push_back({attack, startTime, false});

    // 根據開始時間排序
    std::sort(m_Attacks.begin(), m_Attacks.end(),
              [](const AttackItem& a, const AttackItem& b) {
                  return a.startTime < b.startTime;
              });

    // 更新總持續時間
    float attackEndTime = startTime + attack->GetDelay() + 0.5f; // 加上攻擊持續時間
    if (attackEndTime > m_TotalDuration) {
        m_TotalDuration = attackEndTime;
    }
}

void AttackPattern::AddEnemyMovement(const EnemyMovement& movement, float startTime) {
    // 新增敵人移動到列表中
    m_Movements.push_back({movement, startTime, false});

    // 根據開始時間排序
    std::sort(m_Movements.begin(), m_Movements.end(),
              [](const MovementItem& a, const MovementItem& b) {
                  return a.startTime < b.startTime;
              });
}

void AttackPattern::Start(std::shared_ptr<Enemy> enemy) {
    if (m_State != State::IDLE) {
        LOG_DEBUG("Attack pattern already running or finished");
        return;
    }

    m_Enemy = enemy;
    m_State = State::RUNNING;
    m_ElapsedTime = 0.0f;

    // 重置所有攻擊和移動的狀態
    for (auto& item : m_Attacks) {
        item.started = false;
    }

    for (auto& item : m_Movements) {
        item.executed = false;
    }

    LOG_DEBUG("Attack pattern started");
}

void AttackPattern::Stop() {
    if (m_State != State::RUNNING) {
        return;
    }

    m_State = State::FINISHED;
    LOG_DEBUG("Attack pattern stopped");
}

void AttackPattern::Update(float deltaTime, std::shared_ptr<Character> player) {
    if (m_State != State::RUNNING) {
        return;
    }

    // 更新經過的時間
    m_ElapsedTime += deltaTime;

    // 檢查是否達到總持續時間
    if (m_ElapsedTime >= m_TotalDuration) {
        m_State = State::FINISHED;
        LOG_DEBUG("Attack pattern completed");
        return;
    }

    // 更新攻擊
    for (auto& item : m_Attacks) {
        // 檢查是否到達攻擊開始時間
        if (!item.started && m_ElapsedTime >= item.startTime) {
            item.started = true;
            item.attack->SetTargetCharacter(player);
            LOG_DEBUG("Starting attack at time {}", m_ElapsedTime);
        }

        // 更新已開始的攻擊
        if (item.started && !item.attack->IsFinished()) {
            // 只需要更新攻擊，碰撞檢測將在 OnAttackUpdate 中進行
            item.attack->Update(deltaTime);
        }
    }

    // 執行敵人移動
    for (auto& item : m_Movements) {
        // 檢查是否到達移動開始時間
        if (!item.executed && m_ElapsedTime >= item.startTime) {
            item.executed = true;

            // 執行移動函數
            if (m_Enemy) {
                item.movement(m_Enemy);
                LOG_DEBUG("Executing enemy movement at time {}", m_ElapsedTime);
            }
        }
    }
}