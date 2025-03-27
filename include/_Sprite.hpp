#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "pch.hpp"
#include "Util/Image.hpp"

struct SpriteFrame {
    std::shared_ptr<Util::Image> image;
    glm::vec2 uvTopLeft;
    glm::vec2 uvBottomRight;
};

class SpriteManager {
public:
    explicit SpriteManager(const std::string& imagePath, float frameWidth, float frameHeight, int framesPerRow)
        : m_Image(std::make_shared<Util::Image>(imagePath)),
          m_FrameWidth(frameWidth), m_FrameHeight(frameHeight),
          m_FramesPerRow(framesPerRow) {}

    /**
     * @brief 取所有幀（使用 UV Mapping 裁切）
     * @param frameCount 這個動作的總幀數
     * @return 該動作所有幀的 SpriteFrame
     */
    [[nodiscard]] std::vector<SpriteFrame> GetAnimationFrames(int frameCount) const {
        std::vector<SpriteFrame> frames;
        glm::vec2 textureSize = m_Image->GetSize();

        for (int i = 0; i < frameCount; i++) {
            float x = i * m_FrameWidth;
            float y = 0.0f;

            // UV Mapping 計算
            glm::vec2 uvTopLeft = { x / textureSize.x, y / textureSize.y };
            glm::vec2 uvBottomRight = { (x + m_FrameWidth) / textureSize.x, (y + m_FrameHeight) / textureSize.y };

            frames.push_back({ m_Image, uvTopLeft, uvBottomRight });
        }
        return frames;
    }

private:
    std::shared_ptr<Util::Image> m_Image;
    float m_FrameWidth, m_FrameHeight;
    int m_FramesPerRow;
};

#endif //SPRITE_HPP
