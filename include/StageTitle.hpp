#ifndef STAGE_TITLE_HPP
#define STAGE_TITLE_HPP

#include "Enemy.hpp"
#include "Util/Image.hpp"

class StageTitle : public Enemy {
public:
    explicit StageTitle(const int mainPhase) : Enemy("StageTitle", 1.0f, {ImagePath(mainPhase)}) { // 修正建構子
        m_Transform.scale = {0.5f, 0.5f};
        SetZIndex(200);
    }

    void SetStageTitle(const int mainPhase) {
        m_Drawable = std::make_shared<Util::Image>(ImagePath(mainPhase));
    }

private:
    static std::string ImagePath(const int mainPhase) {
        switch (mainPhase) {
            case 0: return GA_RESOURCE_DIR "/Image/StageTitle/stage_title_en_0000.png";
            case 1: return GA_RESOURCE_DIR "/Image/StageTitle/stage_title_en_0001.png";
            case 2: return GA_RESOURCE_DIR "/Image/StageTitle/stage_title_en_0002.png";
            case 3: return GA_RESOURCE_DIR "/Image/StageTitle/stage_title_en_0003.png";
            case 4: return GA_RESOURCE_DIR "/Image/StageTitle/stage_title_en_0004.png";
            case 5: return GA_RESOURCE_DIR "/Image/StageTitle/stage_title_en_0005.png";
            default: return GA_RESOURCE_DIR "/Image/StageTitle/stage_title_en_0000.png";
        }
    }
};

#endif // STAGE_TITLE_HPP