#include "Character.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Util/TransformUtils.hpp"

Character::Character(const std::vector<std::string>& ImagePathSet) {
    // 建立閒置動畫
    m_IdleAnimation = std::make_shared<Util::Animation>(ImagePathSet, true, 250, true, 0);
    // 初始時設置為閒置動畫
    m_Drawable = m_IdleAnimation;
    m_ImagePathSet = ImagePathSet;
    ResetPosition();

    // 初始血量設置
    m_Health = 100;
    m_MaxHealth = 100;

    LOG_INFO("Character created with {} health", m_Health);
}

void Character::AddSkill(int skillId, const std::vector<std::string>& skillImageSet,
                        int duration) {
    // 創建並儲存新技能
    auto newSkill = std::make_shared<Skill>(skillId, skillImageSet, duration);
    m_Skills[skillId] = newSkill;
    LOG_DEBUG("Added skill with ID: " + std::to_string(skillId));
}

void Character::UseSkill(int skillId) {
    // 技能可在任何狀態下使用，包括受傷狀態
    // 檢查技能是否存在
    auto it = m_Skills.find(skillId);
    if (it != m_Skills.end()) {
        LOG_DEBUG("Character using skill with ID: " + std::to_string(skillId));
        SwitchToSkill(skillId);
    } else {
        LOG_DEBUG("Skill with ID " + std::to_string(skillId) + " not found!");
    }
}

void Character::Update() {
    if (m_Invincible) {
        m_InvincibleTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;
        if (m_InvincibleTimer >= m_InvincibleDuration) {
            m_Invincible = false;
            m_InvincibleTimer = 0.0f;
            LOG_DEBUG("Character invincibility ended");
        }
        else {
            // 無敵時間內閃爍效果
        }
    }

    // 基於當前狀態更新角色
    if (m_State == State::USING_SKILL && m_CurrentSkill) {
        // 更新技能
        m_CurrentSkill->Update(Util::Time::GetDeltaTimeMs() / 1000.0f);

        // 檢查技能是否結束
        if (m_CurrentSkill->IsEnded()) {
            SwitchToIdle();
        }
    }
    else if (m_State == State::HURT) {
        // 更新受傷動畫計時器
        m_HurtAnimationTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;

        // 如果受傷動畫結束，切回閒置狀態
        if (m_HurtAnimationTimer >= m_HurtAnimationDuration) {
            SwitchToIdle();
        }
    }
}

void Character::SwitchToIdle() {
    if (m_State != State::IDLE) {
        LOG_DEBUG("Switching to IDLE animation");
        m_State = State::IDLE;
        m_Drawable = m_IdleAnimation; // 直接切換到閒置動畫物件
        m_CurrentSkillId = -1;
        m_CurrentSkill = nullptr;
        m_HurtAnimationTimer = 0.0f;
    }
}

void Character::SwitchToSkill(int skillId) {
    auto it = m_Skills.find(skillId);
    if (it != m_Skills.end()) {
        LOG_DEBUG("Switching to skill animation for skill ID: " + std::to_string(skillId));
        m_State = State::USING_SKILL;
        m_CurrentSkillId = skillId;
        m_CurrentSkill = it->second;

        // 播放技能動畫和效果 傳遞位置
        m_CurrentSkill->Play(m_Transform.translation);
        m_Drawable = m_CurrentSkill->GetAnimation();
    }
}

void Character::SwitchToHurt() {
    LOG_DEBUG("Switching to HURT animation");
    m_State = State::HURT;
    m_HurtAnimationTimer = 0.0f;
    m_Drawable = m_HurtAnimation;
    m_HurtAnimation->Play();
}

bool Character::IfCollides(const std::shared_ptr<Character>& other, float Distance) const{
    if (!other) return false;
    // 取得當前角色和另一個角色的位置
    glm::vec2 pos1 = this->GetPosition();
    glm::vec2 pos2 = other->GetPosition();
    float size = Distance;
    // 簡單的 AABB (Axis-Aligned Bounding Box) 碰撞檢測
    bool isColliding = (abs(pos1.x - pos2.x) < size) && (abs(pos1.y - pos2.y) < size);
    return isColliding;
}

void Character::TakeDamage(int damage) {
    // 如果無敵不受傷
    if (m_Invincible || !IsAlive()) {
        LOG_DEBUG("Character is invincible or already defeated, damage ignored");
        return;
    }

    // 扣血
    m_Health = std::max(0, m_Health - damage);
    LOG_INFO("Character took {} damage, health: {}/{}", damage, m_Health, m_MaxHealth);

    // 進入無敵
    m_Invincible = true;
    m_InvincibleTimer = 0.0f;

    SwitchToHurt();

    // 死亡
    if (m_Health <= 0) {
        LOG_INFO("Character defeated!");
    }
}

void Character::SetMaxHealth(int maxHealth) {
    m_MaxHealth = maxHealth;
    m_Health = std::min(m_Health, m_MaxHealth); // 確保當前血量不超過最大值
    LOG_INFO("Character max health set to {}, current health: {}", m_MaxHealth, m_Health);
}

void Character::AddHurtAnimation(const std::vector<std::string>& hurtImageSet, int duration) {
    // 創建受傷動畫
    m_HurtAnimation = std::make_shared<Util::Animation>(hurtImageSet, true, duration, false, 0);
    m_HurtAnimationDuration = duration / 1000.0f; // 轉換為秒
    LOG_DEBUG("Added hurt animation with duration: {} ms", duration);
}