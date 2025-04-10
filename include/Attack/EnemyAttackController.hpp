// include/Attack/EnemyAttackController.hpp
#ifndef ENEMYATTACKCONTROLLER_HPP
#define ENEMYATTACKCONTROLLER_HPP

#include "Attack/AttackPattern.hpp"
#include "Attack/AttackPatternFactory.hpp"
#include "Enemy.hpp"
#include "Character.hpp"
#include <queue>

/**
 * @class EnemyAttackController
 * @brief 管理敵人攻擊模式序列的控制器
 *
 * 此類別負責存儲和管理敵人的攻擊模式序列，
 * 讓敵人能夠按照預定義的順序執行不同的攻擊模式。
 */
class EnemyAttackController {
public:
    /**
     * @brief 建構函數
     * @param enemy 控制器將管理的敵人
     */
    explicit EnemyAttackController(std::shared_ptr<Enemy> enemy);

    /**
     * @brief 初始化Battle 1的攻擊模式序列
     */
    void InitBattle1Patterns();

    /**
     * @brief 初始化Battle 2的攻擊模式序列
     */
    void InitBattle2Patterns();

    /**
     * @brief 更新控制器和當前執行的攻擊模式
     * @param deltaTime 時間增量
     * @param player 目標玩家
     */
    void Update(float deltaTime, std::shared_ptr<Character> player);

    /**
     * @brief 檢查所有攻擊模式是否已經完成
     * @return 如果所有模式都完成則返回true
     */
    bool IsAllPatternsCompleted() const;

    /**
     * @brief 將新的攻擊模式添加到序列末尾
     * @param pattern 要添加的攻擊模式
     */
    void AddPattern(std::shared_ptr<AttackPattern> pattern);

    /**
     * @brief 清空所有攻擊模式
     */
    void ClearPatterns();

    /**
     * @brief 開始執行攻擊模式序列
     */
    void Start();

    /**
     * @brief 停止當前攻擊模式
     */
    void Stop();

    /**
     * @brief 重置控制器
     */
    void Reset();

private:
    // 切換到下一個攻擊模式
    void SwitchToNextPattern();

    std::shared_ptr<Enemy> m_Enemy;
    std::queue<std::shared_ptr<AttackPattern>> m_PatternQueue;
    std::shared_ptr<AttackPattern> m_CurrentPattern;
    float m_CooldownTime = 2.0f;         // 模式間的冷卻時間
    float m_ElapsedCooldownTime = 0.0f;  // 當前已過的冷卻時間
    bool m_IsInCooldown = false;        // 是否處於冷卻狀態
    bool m_IsActive = false;            // 控制器是否處於活動狀態
};

#endif // ENEMYATTACKCONTROLLER_HPP