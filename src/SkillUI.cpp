#include "SkillUI.hpp"
#include "Util/Logger.hpp"

#include <sstream>
#include <iomanip>

SkillUI::SkillUI(const std::shared_ptr<Character>& character)
    : m_Character(character) {

    constexpr float baseX = -75.0f;
    constexpr float baseY = -250.0f;

    for (int i = 0; i < NUM_SKILLS; ++i) {
        constexpr float spacing = 50.0f;

        m_SkillIcons.emplace_back( std::make_shared<Object>(IconImagePath(i+1)) );
        m_SkillIcons[i] -> SetPosition(glm::vec2(baseX + i * spacing, baseY));
        m_SkillIcons[i] -> m_Transform.scale = {0.38f, 0.38f};
        m_SkillIcons[i] -> SetZIndex(80);

        m_CooldownTexts.emplace_back( std::make_shared<TextObject>(" ") );
        m_CooldownTexts[i] -> SetPosition(glm::vec2(baseX + i * spacing+5, baseY-5));
        // LOG_INFO("CooldownTexts: {} -- {}", i, m_CooldownTexts[i]->GetPosition());
    }
    m_SkillIcons[1] -> MovePosition(glm::vec2(0,1.5));


    for (int i = 0; i < NUM_SKILLS; ++i) {
        m_SkillIcons2.emplace_back( std::make_shared<Object>(IconImagePath(i+1)) );
        m_SkillIcons2[i] -> m_Transform.scale = {0.18f, 0.18f};
        m_SkillIcons2[i] -> SetZIndex(80);

        m_CooldownTexts2.emplace_back( std::make_shared<TextObject>(" ", 18) );
        // LOG_INFO("CooldownTexts: {} -- {}", i, m_CooldownTexts[i]->GetPosition());
    }
}

void SkillUI::Update() {
    if (!m_Character) return;

    if (m_Visible != m_Character->GetVisibility()) {
        SetVisible(!m_Visible);
    }

    for (int i = 0; i < NUM_SKILLS; ++i) {
        int skillId = i + 1;
        float remainingCooldown = m_Character->GetRemainingCooldown(skillId);

        if (remainingCooldown > 0.5f) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(0) << remainingCooldown;
            m_CooldownTexts[i]->SetText(ss.str());
            m_CooldownTexts2[i]->SetText(ss.str());
            // LOG_INFO("CooldownTexts: {} -- {}", i, ss.str());
        } else {
            m_CooldownTexts[i]->SetText(" "); // 冷卻完畢，清除文字
            m_CooldownTexts2[i]->SetText(" "); // 冷卻完畢，清除文字
        }
    }

    IconsFollow();
}

void SkillUI::SetVisible(const bool visible) {
    if (!m_Character) return;

    m_Visible = visible;

    for (int i = 0; i < NUM_SKILLS; ++i) {
        m_SkillIcons[i] -> SetVisible(visible);
        m_CooldownTexts[i] -> SetVisible(visible);

        m_SkillIcons2[i] -> SetVisible(visible);
        m_CooldownTexts2[i] -> SetVisible(visible);
        // LOG_INFO("SetVisible: {} -- {}", i, visible);
    }
}

void SkillUI::IconsFollow() const {
    if (!m_Character) return;
    const float baseX = m_Character->GetPosition().x;
    const float baseY = m_Character->GetPosition().y;

    m_SkillIcons2[0] -> SetPosition(glm::vec2(baseX-75, baseY));
    m_SkillIcons2[1] -> SetPosition(glm::vec2(baseX, baseY-75));
    m_SkillIcons2[2] -> SetPosition(glm::vec2(baseX+75, baseY));
    m_SkillIcons2[3] -> SetPosition(glm::vec2(baseX, baseY+75));

    const float baseX2 = baseX + 7;
    const float baseY2 = baseY - 2;

    m_CooldownTexts2[0] -> SetPosition(glm::vec2(baseX2-75, baseY2));
    m_CooldownTexts2[1] -> SetPosition(glm::vec2(baseX2, baseY2-75));
    m_CooldownTexts2[2] -> SetPosition(glm::vec2(baseX2+75, baseY2));
    m_CooldownTexts2[3] -> SetPosition(glm::vec2(baseX2, baseY2+75));
    // LOG_INFO("SetPosition: {} -- {},{}", "SkillUI::Icons", baseX, baseY2);
}
