#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/logger.hpp"
#include "Util/Time.hpp"
#include "Effect/EffectManager.hpp"
#include "Effect/EffectFactory.hpp"
#include "Attack/EnemyAttackController.hpp"
#include "Attack/AttackManager.hpp"
#include "Attack/CircleAttack.hpp"
#include "Attack/RectangleAttack.hpp"

void App::Update() {
    // 獲取時間增量
    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

    if (!m_IsReady) {
        GetReady();
        return;
    }
    if (m_PRM->GetCurrentMainPhase()==1) {
        m_GetReady->SetVisible(false);
        m_Enemy_dummy->SetVisible(false);
    }
    if (m_PausedOption->GetVisibility() == true) {
        Pause();
        return;
    }


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
    constexpr float moveSpeed = 6.0f; // 調整移動速度
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

    // 初始化敵人容器
    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    m_Enemies.push_back(m_Enemy);
    m_Enemies.push_back(m_Enemy_bird_valedictorian);
    m_Enemies.push_back(m_Enemy_dragon_silver);
    m_Enemies.push_back(m_Enemy_treasure);
    std::vector<std::shared_ptr<Character>> m_enemies_characters;
    for (const auto& enemy : m_Enemies) {
        m_enemies_characters.push_back(enemy); // 隱式轉換 std::shared_ptr<Enemy> 到 std::shared_ptr<Character>
    }

    // 技能Z
    if (m_ZKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::Z)) {
            LOG_DEBUG("Z Key UP - Skill 1");
            if (m_Rabbit->UseSkill(1)) {
                for (const auto& enemy : m_Enemies) {// 遍歷範圍內的敵人
                    if (m_Rabbit->IfCollides(enemy, 200)) {
                        enemy->TakeDamage(10005);
                    }
                }
                m_Rabbit -> TowardNearestEnemy(m_enemies_characters);
            }
        }
    }
    m_ZKeyDown = Util::Input::IsKeyPressed(Util::Keycode::Z);

    // 技能X
    if (m_XKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::X)) {
            LOG_DEBUG("X Key UP - Skill 2");
            if (m_Rabbit->UseSkill(2)) {
                for (const auto& enemy : m_Enemies) {// 遍歷範圍內的敵人
                    if (m_Rabbit->IfCollides(enemy, 200)) {
                        enemy->TakeDamage(5);
                    }
                }
                m_Rabbit -> TowardNearestEnemy(m_enemies_characters);
            }
        }
    }
    m_XKeyDown = Util::Input::IsKeyPressed(Util::Keycode::X);

    // 技能C
    if (m_CKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::C)) {
            LOG_DEBUG("C Key UP - Skill 3");
            if (m_Rabbit->UseSkill(3)) {
                for (const auto& enemy : m_Enemies) {// 遍歷範圍內的敵人
                    if (m_Rabbit->IfCollides(enemy, 200)) {
                        enemy->TakeDamage(25);
                    }
                }
                m_Rabbit -> TowardNearestEnemy(m_enemies_characters);
            }
        }
    }
    m_CKeyDown = Util::Input::IsKeyPressed(Util::Keycode::C);

    // 技能V
    if (m_VKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::V)) {
            LOG_DEBUG("V Key UP - Skill 4");
            if (m_Rabbit->UseSkill(4)) {
                for (const auto& enemy : m_Enemies) {// 遍歷範圍內的敵人
                    if (m_Rabbit->IfCollides(enemy, 200)) {
                        enemy->TakeDamage(55);
                    }
                }
                m_Rabbit -> TowardNearestEnemy(m_enemies_characters);
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

    // 更新攻擊管理器 (新增)
    AttackManager::GetInstance().Update(deltaTime, m_Rabbit);

    // 更新特效管理器
    Effect::EffectManager::GetInstance().Update(deltaTime);

    // 更新兔子角色
    m_Rabbit->Update();

    // 更新敵人血條，是否允許(前進)
    for (const auto& enemy : m_Enemies) {// 遍歷範圍內的敵人
        enemy->DrawHealthBar();
    }
    if (Enemy::s_HealthBarYPositions.empty()) {
        m_Onward->SetVisible(true);
    } else {
        m_Onward->SetVisible(false);
        Enemy::s_HealthBarYPositions.clear();
    }

    ValidTask();

    // 更新敵人角色
    m_Enemy->Update();
    m_PRM->Update();

    m_Enemy_dummy->Update();
    m_SkillUI->Update();


    // 測試
    if (m_NKeyDown) {
        if (!Util::Input::IsKeyPressed(Util::Keycode::N)) {
            Pause();
        }
    }
    m_NKeyDown = Util::Input::IsKeyPressed(Util::Keycode::N);




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
    // 測試矩形雷射特效 - 按下 1 鍵
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_1)) {
        auto cursorPos = Util::Input::GetCursorPosition();
        auto effect = Effect::EffectManager::GetInstance().PlayEffect(
            Effect::EffectType::RECT_LASER,
            cursorPos,
            20.0f, // z-index
            2.0f   // 持續時間
        );
        LOG_DEBUG("Created RECT_LASER effect at position: ({}, {})", cursorPos.x, cursorPos.y);
    }

    // 測試矩形光束特效 - 按下 2 鍵
    if (Util::Input::IsKeyDown(Util::Keycode::NUM_2)) {
        auto cursorPos = Util::Input::GetCursorPosition();

        // 獲取標準自動旋轉特效
        auto effect1 = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_BEAM);
        auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(effect1->GetBaseShape());
        rectangleShape->SetRotation(0.0f);
        effect1->SetDuration(10.0f);
        effect1->Play(cursorPos, 20.0f);
        LOG_DEBUG("Created standard auto-rotating RECT_BEAM effect at position: ({}, {})", cursorPos.x, cursorPos.y);

        // 創建一個新的自定義光束特效 - 從工廠獲取類似的基本特效
        auto effect2 = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_BEAM);
        if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(effect2->GetBaseShape())) {
            // 設置90度旋轉 (π/2 弧度)
            rectangleShape->SetRotation(1.57f);
            effect2->SetDuration(10.0f);
            // 可選：調整旋轉速度
            // rectangleShape->SetAutoRotation(true, 1.0f);  // 減慢旋轉速度
        }

        // 放置在與第一個光束有點偏移的位置
        effect2->Play(cursorPos, 25.0f);
    }

    // 更新所有渲染對象
    m_Root.Update();
}