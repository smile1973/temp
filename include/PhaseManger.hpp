#ifndef PHASE_MANAGER_HPP
#define PHASE_MANAGER_HPP

#include "Util/GameObject.hpp"
#include "Character.hpp"
#include "Background.hpp"

/**
 * @class PhaseManager
 * @brief 管理遊戲階段的類別。
 */
class PhaseManager {
public:
    PhaseManager();

    /**
     * @brief 取得場景中的子物件。
     * @return 共享指標陣列，包含背景物件。
     */
    [[nodiscard]] std::vector<std::shared_ptr<Util::GameObject>> GetChildren() const {
        return {m_Background};
    }

    /**
     * @brief 進入下一階段。
     */
    void NextPhase();

private:
    std::shared_ptr<BackgroundImage> m_Background; ///< 背景物件
    int m_Phase = 1; ///< 當前階段計數器
};

#endif // PHASE_MANAGER_HPP