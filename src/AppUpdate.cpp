#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/logger.hpp"
#include "Util/Time.hpp"
#include "Effect/EffectManager.hpp"
#include "Effect/EffectFactory.hpp"
#include "Attack/EnemyAttackController.hpp"
#include "Attack/CircleAttack.hpp"
#include "Attack/RectangleAttack.hpp"

void App::Update() {
    // 獲取時間增量
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    // 處理空格鍵 - 測試特效
    if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
        auto cursorPos = Util::Input::GetCursorPosition();

        Effect::EffectType effectType;
        effectType = Effect::EffectType::ENEMY_ATTACK_1;
        LOG_DEBUG("Testing enemy attack 1 effect");

        auto effect = Effect::EffectManager::GetInstance().PlayEffect(
            effectType,
            cursorPos,
            10.0f, // z-index
            1.5f   // 持續時間
        );
        LOG_DEBUG("Created effect at cursor position: ({}, {})", cursorPos.x, cursorPos.y);
    }

    // 角色移動
    const float moveSpeed = 6.0f; // 調整移動速度
    auto rabbitPos = m_Rabbit->GetPosition(); // 取得當前位置

    if (Util::Input::IsKeyPressed(Util::Keycode::UP)) {
        rabbitPos.y += moveSpeed; // 向上移動
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::DOWN)) {
        rabbitPos.y -= moveSpeed; // 向下移動
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        rabbitPos.x -= moveSpeed; // 向左移動
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        rabbitPos.x += moveSpeed; // 向右移動
    }
    m_Rabbit->SetPosition(rabbitPos); // 更新位置

    // 退出檢查
    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    // 技能Z
    if (m_ZKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::Z)) {
            LOG_DEBUG("Z Key UP - Skill 1");
            m_Rabbit->UseSkill(1);

            if (m_Rabbit->IfCollides(m_Enemy, 200)) {
                m_Enemy->TakeDamage(5);
                m_Enemy->MovePosition(glm::vec2(-10.0f,3.0f));
            }
        }
    }
    m_ZKeyDown = Util::Input::IsKeyPressed(Util::Keycode::Z);

    // 技能X
    if (m_XKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::X)) {
            LOG_DEBUG("X Key UP - Skill 2");
            m_Rabbit->UseSkill(2);
        }
    }
    m_XKeyDown = Util::Input::IsKeyPressed(Util::Keycode::X);

    // 技能C
    if (m_CKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::C)) {
            LOG_DEBUG("C Key UP - Skill 3");
            m_Rabbit->UseSkill(3);
        }
    }
    m_CKeyDown = Util::Input::IsKeyPressed(Util::Keycode::C);

    // 技能V
    if (m_VKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::V)) {
            LOG_DEBUG("V Key UP - Skill 4");
            m_Rabbit->UseSkill(4);

            if (m_Rabbit->IfCollides(m_Enemy, 200)) {
                m_Enemy->TakeDamage(15);
            }
        }
    }
    m_VKeyDown = Util::Input::IsKeyPressed(Util::Keycode::V);

    // 更新攻擊控制器 (如果處於活動狀態)
    if (m_EnemyAttackController && m_Enemy->GetVisibility()) {
        m_EnemyAttackController->Update(deltaTime, m_Rabbit);

        // 檢查是否所有攻擊模式都已完成，如果是則循環
        if (m_EnemyAttackController->IsAllPatternsCompleted()) {
            // 根據當前關卡重新初始化攻擊模式
            if (m_Phase == Phase::BATTLE_1) {
                m_EnemyAttackController->InitBattle1Patterns();
            } else if (m_Phase == Phase::BATTLE_2) {
                m_EnemyAttackController->InitBattle2Patterns();
            }

            // 重新啟動攻擊控制器
            m_EnemyAttackController->Start();
        }
    }

    // 更新特效管理器
    Effect::EffectManager::GetInstance().Update(deltaTime);

    // 更新兔子角色
    m_Rabbit->Update();

    // 更新敵人血條
    if (m_Enemy->GetVisibility()){
        m_Enemy->DrawHealthBar(glm::vec2 (0.9f, 0.9));  // 繪製血條
        m_Onward->SetVisible(false);
    } else {
        m_Onward->SetVisible(true);
    }

    // 按I鍵測試多個敵人攻擊特效
    if (Util::Input::IsKeyDown(Util::Keycode::I)) {
        for (int i = 0; i < 3; ++i) {
            auto eff = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::ENEMY_ATTACK_1);
            eff->SetMovementModifier(Effect::Modifier::MovementModifier(true, 250.0f, 1200.0f, {0.0f, -1.0f}));
            eff->SetDuration(5.0f);
            eff->Play({-500 + (500 * i), 500}, 30.0f);
        }
    }

    // 按B鍵手動啟動Battle 1攻擊模式
    if (Util::Input::IsKeyDown(Util::Keycode::B)) {
        if (m_EnemyAttackController) {
            m_EnemyAttackController->InitBattle1Patterns();
            m_EnemyAttackController->Start();
            LOG_DEBUG("Manual start Battle 1 attack patterns");
        }
    }

    // 按M鍵手動啟動Battle 2攻擊模式
    if (Util::Input::IsKeyDown(Util::Keycode::M)) {
        if (m_EnemyAttackController) {
            m_EnemyAttackController->InitBattle2Patterns();
            m_EnemyAttackController->Start();
            LOG_DEBUG("Manual start Battle 2 attack patterns");
        }
    }

    // 關卡跳轉測試
    if (m_NKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::N)) {
            ValidTask();
        }
    }
    m_NKeyDown = Util::Input::IsKeyPressed(Util::Keycode::N);
    if (m_Onward->GetVisibility() && m_Rabbit->IfCollides(m_Onward, 80)) {
        ValidTask();
    }

    // 更新所有渲染對象
    m_Root.Update();
}