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
                        int duration, float Cooldown) {
    // 創建並儲存新技能
    auto newSkill = std::make_shared<Skill>(skillId, skillImageSet, duration, Cooldown);
    m_Skills[skillId] = newSkill;
    LOG_DEBUG("Added skill with ID: " + std::to_string(skillId));
}

bool Character::UseSkill(const int skillId) {
    if (m_State == State::IDLE) {
        // 檢查技能是否存在
        auto it = m_Skills.find(skillId);
        if (it != m_Skills.end()) {
            // LOG_DEBUG("Character using skill with ID: " + std::to_string(skillId));
            // SwitchToSkill(skillId);
            if (!it->second->IsOnCooldown()) {
                LOG_DEBUG("Character using skill with ID: " + std::to_string(skillId));
                SwitchToSkill(skillId);
                return true;
            }
            LOG_DEBUG("Skill with ID " + std::to_string(skillId) + " is on cooldown! " + std::to_string(it->second->GetRemainingCooldown()));
        } else {
            LOG_DEBUG("Skill with ID " + std::to_string(skillId) + " not found!");
        }
    }
    return false;
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
    // if (m_State == State::USING_SKILL && m_CurrentSkill) {
    //     // 更新技能
    //     m_CurrentSkill->Update(Util::Time::GetDeltaTimeMs() / 1000.0f);

    // 更新技能
    for (auto it = m_Skills.begin(); it != m_Skills.end(); ++it) {
        it->second->Update(Util::Time::GetDeltaTimeMs() / 1000.0f);
    }
    if (m_State == State::USING_SKILL && m_CurrentSkill) {
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

    // 移動位置
    if (m_IsMoving) {
        // 計算移動距離
        const float DeltaTimeMs = Util::Time::GetDeltaTimeMs();
        m_TotalTime -= DeltaTimeMs;
        // 更新位置
        m_Transform.translation += m_MoveSpeed * DeltaTimeMs / 1000.0f;
        if (m_TotalTime < 0.0f) {
            m_IsMoving = false; // 停止移動
            m_Transform.translation = m_TargetPosition;
            m_TotalTime = 0;
            LOG_DEBUG("Character move to {}", m_Transform.translation);
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


void Character::TowardNearestEnemy(const std::vector<std::shared_ptr<Character>>& m_Enemies) {
    if (m_Enemies.empty()) return;

    float minDistance = std::numeric_limits<float>::max();
    std::shared_ptr<Character> nearestEnemy = nullptr;
    const glm::vec2 currentPosition = this->GetPosition();

    for (const auto& enemy : m_Enemies) {
        if (enemy->GetVisibility()) {
            float distance = glm::distance(currentPosition, enemy->GetPosition());
            if (distance < minDistance) {
                minDistance = distance;
                nearestEnemy = enemy;
            }
        }
    }

    if (nearestEnemy) {
        if (nearestEnemy->GetPosition().x > currentPosition.x) {
            LOG_DEBUG("Towards the Right");
            m_Transform.scale.x = 0.5f;
        } else {
            LOG_DEBUG("Towards the Left");
            m_Transform.scale.x = -0.5f;
        }
    }
}


bool Character::IsSkillOnCooldown(int skillId) const {
    auto it = m_Skills.find(skillId);
    if (it != m_Skills.end()) {
        return it->second->IsOnCooldown();
    }
    return false;
}

void Character::MovePosition(const glm::vec2& Position, const float totalTime) {
    MoveToPosition(GetPosition() + Position, totalTime);
}

void Character::MoveToPosition(const glm::vec2& targetPosition, const float totalTime) {
    if (totalTime <= 0.0f) {
        m_Transform.translation = targetPosition;
        return;
    }

    m_IsMoving = true;
    m_TargetPosition = targetPosition;
    // m_MaxDistance = glm::distance(this->GetPosition(), targetPosition);
    // m_Direction = (targetPosition - this->GetPosition()) / m_MaxDistance;
    // m_Speed = m_MaxDistance / totalTime;
    m_MoveSpeed = (targetPosition - this->GetPosition()) / totalTime;
    m_TotalTime = totalTime * 1000.0f; //(ms)
    LOG_DEBUG("Move Character to {}", m_Transform.translation);
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