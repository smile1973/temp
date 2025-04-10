#ifndef PAUSE_SCREEN_HPP
#define PAUSE_SCREEN_HPP

#include "Util/GameObject.hpp"
#include "Enemy.hpp"

class PausedScreen {
public:
    PausedScreen() {
        std::vector<std::string> optionNames = {"continue", "restart", "manage_player", "game_setting", "return_title_page"};
        for (int i = 0; i < optionNames.size(); ++i) {
            m_Options.push_back(std::make_shared<Enemy>(optionNames[i],1,std::vector{ ImagePath(optionNames[i])}));
            m_Options[i] -> m_Transform.scale =  {1.05f, 1.05f};
            m_Options[i] -> SetZIndex(100);
        }
        m_Options[0] -> m_Transform.scale =  {1.2f, 1.2f};
        m_Options[0] -> SetPosition(m_BasePosition + glm::vec2{0,0});
        m_Options[1] -> SetPosition(m_BasePosition + glm::vec2{-30,-120});
        m_Options[2] -> SetPosition(m_BasePosition + glm::vec2{-70,-222});
        m_Options[3] -> SetPosition(m_BasePosition + glm::vec2{-130,-305});
        m_Options[4] -> SetPosition(m_BasePosition + glm::vec2{-200,-388});

    }

    [[nodiscard]] std::vector<std::shared_ptr<Util::GameObject>> GetChildren() const {
        std::vector<std::shared_ptr<Util::GameObject>> children;
        for (const auto& option : m_Options) {
            children.push_back(option);
        }
        return children;
    }

    void SetVisible(const bool visible) {
        m_IfVisible = visible;
        for (const auto& option : m_Options) {
            option->SetVisible(m_IfVisible);
        }
    }

    [[nodiscard]] int GetCurrentOption() const{ return m_CurrentOption; }

    [[nodiscard]] bool GetVisibility() const{ return m_IfVisible; }

    void Switch (const bool isUp){
        constexpr int maxOption = 4;
        m_Options[m_CurrentOption]->m_Transform.scale =  {1.05f, 1.05f};
        m_CurrentOption = isUp ? (m_CurrentOption == 0) ? maxOption : m_CurrentOption - 1
                               : (m_CurrentOption == maxOption) ? 0 : m_CurrentOption + 1;
        m_Options[m_CurrentOption]->m_Transform.scale =  {1.2f, 1.2f};
    }

    void Reset() {
        m_CurrentOption = 4;
        Switch(false);
    }

    void Update() const {
        for (const auto& option : m_Options) {
            option->Update();
        }
    }

private:
    bool m_IfVisible = false;
    glm::vec2 m_BasePosition = {373, 120};
    std::vector<std::shared_ptr<Enemy>> m_Options;
    int m_CurrentOption = 0;

    static std::string ImagePath(const std::string& imageName) {
        return GA_RESOURCE_DIR "/Image/PausedOption/" + imageName + ".png";
    }

    static std::string BrightImagePath(const std::string& imageName) {
        return GA_RESOURCE_DIR "/Image/PausedOption/" + imageName + "_b.png";
    }
};

#endif // PAUSE_SCREEN_HPP