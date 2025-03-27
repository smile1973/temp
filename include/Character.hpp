#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Skill.hpp"

class Character : public Util::GameObject {
public:
    explicit Character(const std::vector<std::string>& ImageSet);
    enum class State {
        IDLE,
        USING_SKILL,
        HURT      // 新增受傷狀態
    };

    Character(const Character&) = delete;
    Character(Character&&) = delete;
    Character& operator=(const Character&) = delete;
    Character& operator=(Character&&) = delete;

    [[nodiscard]] const std::vector<std::string>& GetImagePathSet() const { return m_ImagePathSet; }
    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool GetVisibility() const { return m_Visible; }

    bool IfCollides(const std::shared_ptr<Character>& other, float Distance) const;

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }

    // 技能
    void AddSkill(int skillId, const std::vector<std::string>& skillImageSet,
                 int duration = 175);
    void UseSkill(int skillId);  // 1=Z, 2=X, 3=C, 4=V
    void Update();

    // 血量相關
    void TakeDamage(int damage = 1);
    bool IsInvincible() const { return m_Invincible; }
    int GetHealth() const { return m_Health; }
    int GetMaxHealth() const { return m_MaxHealth; }
    void SetMaxHealth(int maxHealth);
    bool IsAlive() const { return m_Health > 0; }

    void AddHurtAnimation(const std::vector<std::string>& hurtImageSet, int duration = 500);

private:
    void ResetPosition() { m_Transform.translation = {0, 0}; }
    void SwitchToIdle();
    void SwitchToSkill(int skillId);
    void SwitchToHurt();    // 切換到受傷狀態

    std::vector<std::string> m_ImagePathSet;
    std::shared_ptr<Util::Animation> m_IdleAnimation;
    std::shared_ptr<Util::Animation> m_HurtAnimation;  // 受傷動畫

    // 存所有技能
    std::unordered_map<int, std::shared_ptr<Skill>> m_Skills;

    State m_State = State::IDLE;
    int m_CurrentSkillId = -1;
    std::shared_ptr<Skill> m_CurrentSkill = nullptr;

    // 血量相關屬性
    int m_Health = 100;          // 當前血量
    int m_MaxHealth = 100;       // 最大血量
    bool m_Invincible = false; // 是否處於無敵狀態
    float m_InvincibleTimer = 0.0f; // 無敵時間計時器
    float m_InvincibleDuration = 1.5f; // 無敵時間(秒)

    // 受傷動畫相關
    float m_HurtAnimationTimer = 0.0f;
    float m_HurtAnimationDuration = 0.5f; // 受傷動畫持續時間(秒)
};


#endif //CHARACTER_HPP