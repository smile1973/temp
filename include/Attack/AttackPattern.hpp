// include/AttackPattern.hpp
#ifndef ATTACKPATTERN_HPP
#define ATTACKPATTERN_HPP

#include <vector>
#include <memory>
#include <functional>
#include "Attack.hpp"
#include "Enemy.hpp"

class AttackPattern {
public:
    enum class State {
        IDLE,       // 待機狀態
        RUNNING,    // 執行中
        FINISHED    // 完成
    };

    // 敵人移動指令類型
    using EnemyMovement = std::function<void(std::shared_ptr<Enemy>)>;

    // 建構函數
    AttackPattern();
    virtual ~AttackPattern() = default;

    // 添加攻擊
    void AddAttack(std::shared_ptr<Attack> attack, float startTime);

    // 添加敵人移動
    void AddEnemyMovement(const EnemyMovement& movement, float startTime);

    // 啟動攻擊模式
    void Start(std::shared_ptr<Enemy> enemy);

    // 停止攻擊模式
    void Stop();

    // 更新攻擊模式
    void Update(float deltaTime, std::shared_ptr<Character> player);

    // 檢查是否完成
    bool IsFinished() const { return m_State == State::FINISHED; }

    // 獲取當前狀態
    State GetState() const { return m_State; }

    // 設置總持續時間
    void SetDuration(float duration) { m_TotalDuration = duration; }

    // 獲取總持續時間
    float GetDuration() const { return m_TotalDuration; }

private:
    // 攻擊項結構
    struct AttackItem {
        std::shared_ptr<Attack> attack;
        float startTime;
        bool started = false;
    };

    // 敵人移動項結構
    struct MovementItem {
        EnemyMovement movement;
        float startTime;
        bool executed = false;
    };

    // 成員變數
    State m_State = State::IDLE;
    float m_ElapsedTime = 0.0f;
    float m_TotalDuration = 0.0f;

    std::vector<AttackItem> m_Attacks;
    std::vector<MovementItem> m_Movements;
    std::shared_ptr<Enemy> m_Enemy;
};

#endif //ATTACKPATTERN_HPP
