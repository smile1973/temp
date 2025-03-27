
#ifndef BACKGROUND_IMAGE_HPP
#define BACKGROUND_IMAGE_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

/**
 * @class BackgroundImage
 * @brief 負責管理遊戲背景圖的類別。
 */
class BackgroundImage : public Util::GameObject {
public:
    /**
     * @brief 背景圖片建構函式，預設為一般背景。
     */
    BackgroundImage() : GameObject(
        std::make_unique<Util::Image>(GA_RESOURCE_DIR "/Image/Background/bg_black.png"), -10) {
    }

    /**
     * @brief 根據當前階段更換背景圖片。
     * @param phase 當前遊戲階段。
     */
    void NextPhase(int phase) const{
        auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
        temp->SetImage(ImagePath(phase));
    }

private:
    /**
     * @brief 取得對應階段的背景圖片路徑。
     * @param phase 當前遊戲階段。
     * @return 對應的圖片路徑。
     */
    static std::string ImagePath(int phase) {
        return (phase == 1) ? GA_RESOURCE_DIR "/Image/Background/bg.png" :
                  (phase == 2) ? GA_RESOURCE_DIR "/Image/Background/bg_black.png" :
                  (phase % 2 == 1) ? GA_RESOURCE_DIR "/Image/Background/bg.png" :
                  (phase % 2 == 0) ? GA_RESOURCE_DIR "/Image/Background/bg_black.png" : GA_RESOURCE_DIR "/Image/Background/bg.png";
    }
};

#endif // BACKGROUND_IMAGE_HPP