#include "Attack/AttackManager.hpp"
#include "Util/Logger.hpp"

AttackManager& AttackManager::GetInstance() {
    static AttackManager instance;
    return instance;
}

void AttackManager::RegisterAttack(std::shared_ptr<Attack> attack) {
    if (!attack) {
        LOG_ERROR("Attempted to register null attack");
        return;
    }

    m_ActiveAttacks.push_back(attack);
    LOG_DEBUG("Registered attack, total active attacks: {}", m_ActiveAttacks.size());
}

void AttackManager::Update(float deltaTime, std::shared_ptr<Character> player) {
    // 更新所有攻擊並檢測碰撞
    for (auto it = m_ActiveAttacks.begin(); it != m_ActiveAttacks.end();) {
        auto& attack = *it;
        
        // 更新攻擊
        attack->Update(deltaTime);
        
        // 檢查碰撞
        if (player && attack->GetState() == Attack::State::ATTACKING) {
            attack->CheckCollision(player);
        }
        
        // 如果攻擊已完成，從活躍列表中移除
        if (attack->IsFinished()) {
            LOG_DEBUG("Attack completed and removed from manager");
            it = m_ActiveAttacks.erase(it);
        } else {
            ++it;
        }
    }
}

void AttackManager::ClearAllAttacks() {
    for (auto& attack : m_ActiveAttacks) {
        if (attack) {
            // 通過基類接口清理視覺元素
            attack->CleanupVisuals();
        }
    }
    m_ActiveAttacks.clear();
}