#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Util/Renderer.hpp"
#include "Character.hpp"
#include "Enemy.hpp"
// #include "BackGround.hpp"
#include "PhaseManger.hpp" // 階段資源管理
#include "Effect/EffectManager.hpp"
#include "Attack/EnemyAttackController.hpp" // 新增的敵人攻擊控制器

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    [[nodiscard]] State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    // 執行有效的任務，內部函式
    void ValidTask();


    //private:
    enum class Phase {
        START,
        BATTLE_1,
        BATTLE_2,
        BATTLE_3,
        BATTLE_4,
        STORE,
    };


    State m_CurrentState = State::START;
    Phase m_Phase = Phase::START; // 當前階段

    Util::Renderer m_Root;
    // std::shared_ptr<BackgroundImage> m_Background; // 定義背景類
    std::shared_ptr<Character> m_Rabbit; // 定義兔子
    std::shared_ptr<Enemy> m_Enemy;   // 定義敵人
    std::shared_ptr<PhaseManager> m_PRM; // 階段資源管理器
    std::shared_ptr<EnemyAttackController> m_EnemyAttackController; // 敵人攻擊控制器

    bool m_EnterDown = false;
    bool m_ZKeyDown = false;
    bool m_XKeyDown = false;
    bool m_CKeyDown = false;
    bool m_VKeyDown = false;
    float m_TestEffectTimer = 0.0f;

    // 測試關卡切換
    bool m_NKeyDown = false;
    std::shared_ptr<Enemy> m_Onward;
};

#endif