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
    BackgroundImage() : GameObject(
        std::make_unique<Util::Image>(GA_RESOURCE_DIR "/Image/Background/bg_black.png"), -10) {
    }

    void SetBackground(int mainPhase) const {
        auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
        temp->SetImage(ImagePath(mainPhase));
    }

private:
    /**
     * @brief 取得對應大關的背景圖片路徑。
     * @param mainPhase 當前遊戲大關。
     * @return 對應的圖片路徑。
     */
    static std::string ImagePath(const int mainPhase) {
        switch (mainPhase) {
            case 1: return GA_RESOURCE_DIR "/Image/Background/bg_0.png";
            case 2: return GA_RESOURCE_DIR "/Image/Background/bg_1.png";
            case 3: return GA_RESOURCE_DIR "/Image/Background/bg_2.png";
            case 4: return GA_RESOURCE_DIR "/Image/Background/bg_3.png";
            case 5: return GA_RESOURCE_DIR "/Image/Background/bg_4.png";
            case 6: return GA_RESOURCE_DIR "/Image/Background/bg_5.png";
            default: return GA_RESOURCE_DIR "/Image/Background/bg_0.png";
        }
    }
};

#endif // BACKGROUND_IMAGE_HPP
