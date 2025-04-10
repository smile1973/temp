#ifndef PHASE_MANAGER_HPP
#define PHASE_MANAGER_HPP

#include "Util/GameObject.hpp"
#include "StageTitle.hpp"
#include "Background.hpp"
#include "ProgressBar.hpp"

/**
 * @class PhaseManager
 * @brief 管理遊戲階段的類別。
 */
class PhaseManager {
public:
    PhaseManager() {
        m_Background = std::make_shared<BackgroundImage>();
        m_MainStageTitle = std::make_shared<StageTitle>(m_MainPhase);
        m_ProgressBar = std::make_shared<ProgressBar>();
    }

    /**
     * @brief 取得場景中的子物件。
     * @return 共享指標陣列，包含背景物件。
     */
    [[nodiscard]] std::vector<std::shared_ptr<Util::GameObject>> GetChildren() const {
        std::vector<std::shared_ptr<Util::GameObject>> children = m_ProgressBar->GetChildren();
        children.push_back(m_Background);
        children.push_back(m_MainStageTitle);
        return children;
    }

    void LeaveSubPhase();

    /**
     * @brief 進入下一小關。
     * @return bool 如果所有小關都完成則返回true，否則返回false。
     */
    void NextSubPhase();

    /**
     * @brief 更新小關類型。
     */
    void UpdateSubPhaseType();

    /**
     * @brief 進入下一大關。
     */
    void NextMainPhase();

    void UpdateStageTitle() const;

    void UpdateProgressBar() const;
    [[nodiscard]] bool IfProgressBarSet() const;

    void SetProgressBarVisible(const bool visible) const { m_ProgressBar->SetVisible(visible); }

    void Update() const;

    /**
     * @brief 獲取當前大關索引。
     * @return 當前大關索引 (0-5)。
     */
    [[nodiscard]] int GetCurrentMainPhase() const { return m_MainPhase; }

    /**
     * @brief 獲取當前小關索引。
     * @return 當前小關索引 (0-5)。
     */
    [[nodiscard]] int GetCurrentSubPhase() const { return m_SubPhase; }

    /**
     * @brief 取得對應小關卡的類型編號。
     * @return 對應的小關卡類型編號 (0-3)。
     */
    [[nodiscard]] int GetCurrentSubPhaseType() const { return m_SubPhaseType; }

    /**
     * @brief 獲取背景對象。
     * @return 背景對象的共享指針。
     */
    [[nodiscard]] std::shared_ptr<BackgroundImage> GetBackground() const { return m_Background; }

private:
    std::shared_ptr<BackgroundImage> m_Background; ///< 背景物件
    std::shared_ptr<StageTitle> m_MainStageTitle; ///< 主標題物件
    std::shared_ptr<ProgressBar> m_ProgressBar; ///< 進度條物件
    int m_MainPhase = 0; ///< 當前大關索引 (0-5)
    int m_SubPhase = -1;  ///< 當前小關索引 (0-4)
    int m_SubPhaseType = 0;  ///< 當前小關類型 (0=STORE, 1=BATTLE, 2=TREASURE, 3=BOSS)
    static constexpr int m_MaxSubPhase  = 5; // 最大小關數量常數
    static constexpr int m_MaxMainPhase = 5; // 最大小關數量常數
    bool m_IfProgressBarSet = false;
    bool m_IfLeaveSubPhase = false;


    /**
     * @brief 取得對應大關卡的名稱。
     * @param MainPhase 大關卡編號。
     * @return 對應的大關卡名稱。
     */
    static std::string GetMainPhaseName(const int MainPhase) {
        switch (MainPhase) {
            case 0: return "INITIAL_SCENE";
            case 1: return "KINGDOM_OUTSKIRTS";
            case 2: return "SCHOLARS_NEST";
            case 3: return "KINGS_ARSENAL";
            case 4: return "RED_DARKHOUSE";
            case 5: return "CHURCHMOUSE_STREETS";
            default: return "Error";
        }
    }
    /**
     * @brief 取得對應小關卡的類型名稱。
     * @param SubPhase 小關卡編號。
     * @return 對應的小關卡類型名稱。
     */
    static std::string GetSubPhaseName(const int SubPhase) {
        switch (SubPhase) {
            case 0: return "STORE";
            case 1: return "BATTLE_1";
            case 2: return "BATTLE_2";
            case 3: return "BATTLE_3";
            case 4: return "TREASURE";
            case 5: return "BOSS";
            default: return "Error";
        }
    }

    // enum class SubPhase {
    //     STORE,
    //     BATTLE_1,
    //     BATTLE_2,
    //     BATTLE_3,
    //     TREASURE,
    //     BOSS
    // };
    // SubPhase m_CurrentSubPhase = SubPhase::STORE;     // 當前小關類型
};

#endif // PHASE_MANAGER_HPP
