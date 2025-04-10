#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Util/Renderer.hpp"
#include "Character.hpp"
#include "Enemy.hpp"
#include "PhaseManger.hpp" // 階段資源管理
#include "PauseScreen.hpp"
#include "SkillUI.hpp"
#include "Effect/EffectManager.hpp"
#include "Attack/EnemyAttackController.hpp" // 敵人攻擊控制器
#include "Attack/AttackManager.hpp" // 新增: 攻擊管理器

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };
    static App& GetInstance() {
        static App instance;
        return instance;
    }

    // 禁止複製和賦值
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    [[nodiscard]] State GetCurrentState() const { return m_CurrentState; }
    std::shared_ptr<Util::GameObject> GetOverlay() const { return m_Overlay; }

    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

    void AddToRoot(const std::shared_ptr<Util::GameObject> &object) {
        m_Root.AddChild(object);
    }
    void RemoveFromRoot(const std::shared_ptr<Util::GameObject> &object) {
        m_Root.RemoveChild(object);
    }

private:
    void GetReady();
    void Pause();

    // 執行有效的任務，內部函式
    void ValidTask();
    void LeavePhase() const;
    void SetSubPhase() const;      // 設置關卡配置
    void SetupStorePhase() const;      // 設置商店關卡配置
    void SetupTreasurePhase() const;      // 設置寶箱關卡配置
    void SetupBattlePhase() const;      // 設置戰鬥關卡配置

    App() : m_CurrentState(State::START),
           m_Phase(Phase::START) {}

    //private:
    enum class Phase {
        START,
        BATTLE_1,
        BATTLE_2,
        BATTLE_3,
        STORE,
    };
    // 暫停指令(好像沒用到)
    enum class PausedOption {
        CONTINUE,
        RESTART,
        MANAGE_PLAYER,
        GAME_SETTING,
        RETURN_TITLE_PAGE,
    };

    static App* s_Instance;

    State m_CurrentState = State::START;
    // PausedOption m_CurrenPausedOption = PausedOption::CONTINUE;
    // MainPhase m_MainPhase = MainPhase::INITIAL_SCENE;  // 當前大關
    // int m_SubPhaseIndex = 0;                           // 當前小關索引 (0-4)
    // SubPhase m_CurrentSubPhase = SubPhase::BATTLE;     // 當前小關類型

    Util::Renderer m_Root;
    std::shared_ptr<PhaseManager> m_PRM; // 階段資源管理器
    std::shared_ptr<EnemyAttackController> m_EnemyAttackController; // 敵人攻擊控制器
    std::shared_ptr<Character> m_Rabbit;               // 定義兔子
    std::shared_ptr<Enemy> m_Enemy_dummy;              // 定義敵人_假人
    std::shared_ptr<Enemy> m_Enemy;                    // 定義敵人
    std::shared_ptr<Enemy> m_Enemy_bird_valedictorian; // 定義敵人
    std::shared_ptr<Enemy> m_Enemy_dragon_silver;      // 定義敵人
    std::shared_ptr<Enemy> m_Enemy_treasure;           // 定義寶箱
    std::shared_ptr<Enemy> m_Enemy_shopkeeper;         // 定義商人
    std::shared_ptr<PausedScreen> m_PausedOption;      // 階段資源管理器
    std::shared_ptr<SkillUI> m_SkillUI;                // 階段資源管理器
    std::shared_ptr<Util::GameObject> m_Overlay;

    bool m_EnterDown = false;
    bool m_ZKeyDown = false;
    bool m_XKeyDown = false;
    bool m_CKeyDown = false;
    bool m_VKeyDown = false;
    float m_TestEffectTimer = 0.0f;

    // 測試關卡切換
    bool m_NKeyDown = false;
    bool m_UpKeyDown = false;
    bool m_DownKeyDown = false;
    std::shared_ptr<Enemy> m_Onward;
    std::shared_ptr<Enemy> m_GetReady;
    std::shared_ptr<Enemy> m_PressZtoJoin;
    bool m_IsReady = false;
    int m_CurrentPausedOption = 0;
    Phase m_Phase;
};

#endif