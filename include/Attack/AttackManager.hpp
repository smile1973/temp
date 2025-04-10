#ifndef ATTACKMANAGER_HPP
#define ATTACKMANAGER_HPP

#include <memory>
#include <vector>
#include "Attack/Attack.hpp"
#include "Character.hpp"

/**
 * @class AttackManager
 * @brief 管理所有遊戲中的攻擊物件
 * 
 * 此類別負責集中管理所有活躍的攻擊物件，處理攻擊的更新和碰撞檢測。
 * 攻擊模式只負責生成攻擊，生成後的管理由此類別負責。
 */
class AttackManager {
public:
    /**
     * @brief 獲取單例實例
     * @return 單例實例的引用
     */
    static AttackManager& GetInstance();

    // 禁止複製和賦值
    AttackManager(const AttackManager&) = delete;
    AttackManager& operator=(const AttackManager&) = delete;

    /**
     * @brief 註冊一個攻擊物件到管理器
     * @param attack 要註冊的攻擊物件
     */
    void RegisterAttack(std::shared_ptr<Attack> attack);

    /**
     * @brief 更新所有攻擊並檢測碰撞
     * @param deltaTime 時間增量
     * @param player 玩家角色指針，用於碰撞檢測
     */
    void Update(float deltaTime, std::shared_ptr<Character> player);

    /**
     * @brief 清除所有活躍的攻擊
     */
    void ClearAllAttacks();

    /**
     * @brief 獲取當前活躍的攻擊數量
     * @return 活躍攻擊數量
     */
    size_t GetActiveAttacksCount() const { return m_ActiveAttacks.size(); }

private:
    // 私有構造函數，確保單例模式
    AttackManager() = default;

    // 所有活躍的攻擊物件
    std::vector<std::shared_ptr<Attack>> m_ActiveAttacks;
};

#endif // ATTACKMANAGER_HPP