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
    };

    Character(const Character&) = delete;
    Character(Character&&) = delete;
    Character& operator=(const Character&) = delete;
    Character& operator=(Character&&) = delete;

    [[nodiscard]] const std::vector<std::string>& GetImagePathSet() const { return m_ImagePathSet; }
    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool GetVisibility() const { return m_Visible; }
    // 檢測角色是否與另一個角色發生碰撞
    bool IfCollides(const std::shared_ptr<Character>& other, float Distance) const;

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }

    // 技能
    void AddSkill(int skillId, const std::vector<std::string>& skillImageSet,
                 int duration = 175);
    void UseSkill(int skillId);  // 1=Z, 2=X, 3=C, 4=V
    void Update();

private:
    void ResetPosition() { m_Transform.translation = {0, 0}; }
    void SwitchToIdle();
    void SwitchToSkill(int skillId);

    std::vector<std::string> m_ImagePathSet;
    std::shared_ptr<Util::Animation> m_IdleAnimation;

    // 存所有技能
    std::unordered_map<int, std::shared_ptr<Skill>> m_Skills;

    State m_State = State::IDLE;
    int m_CurrentSkillId = -1;
    std::shared_ptr<Skill> m_CurrentSkill = nullptr;
};


#endif //CHARACTER_HPP
