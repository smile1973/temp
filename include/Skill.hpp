#ifndef SKILL_HPP
#define SKILL_HPP

#include "Util/Animation.hpp"
#include "Effect/EffectManager.hpp"
#include "Util/Color.hpp"

class Skill {
public:
    enum class State {
        IDLE,
        ACTIVE
    };

    // 構造函數
    Skill(int skillId, const std::vector<std::string>& imageSet, int duration = 175);

    // 取得動畫物件
    std::shared_ptr<Util::Animation> GetAnimation() const { return m_Animation; }

    // 播放技能動畫和效果
    void Play(const glm::vec2& position);

    // 檢查動畫是否已結束
    bool IsEnded() const;

    // 取得動畫路徑集
    const std::vector<std::string>& GetImagePathSet() const { return m_ImagePathSet; }

    // 更新技能狀態
    void Update(float deltaTime);

    // 取得技能當前狀態
    State GetState() const { return m_State; }

    // 設置效果的參數
    void SetEffectRadius(float radius) { m_EffectRadius = radius; }
    void SetEffectColor(const Util::Color& color) { m_EffectColor = color; }

private:
    std::vector<std::string> m_ImagePathSet;
    std::shared_ptr<Util::Animation> m_Animation;
    State m_State = State::IDLE;
    int m_Duration = 175; // 技能動畫持續時間（毫秒）
    int m_SkillId = 1;

    // 特效參數
    float m_EffectRadius = 0.4f;
    Util::Color m_EffectColor = Util::Color::FromName(Util::Colors::WHITE);
    glm::vec2 m_EffectSize = {800, 800};
    std::shared_ptr<Effect::IEffect> m_CurrentEffect = nullptr; // 只是用在追蹤 看之後要不要用
};

#endif //SKILL_HPP