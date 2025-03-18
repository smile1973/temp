// include/Attack/RectangleAttack.hpp
#ifndef RECTANGLEATTACK_HPP
#define RECTANGLEATTACK_HPP

#include "Attack/Attack.hpp"

/**
 * @class RectangleAttack
 * @brief 矩形攻擊實現，用於創建矩形的攻擊區域
 */
class RectangleAttack : public Attack {
public:
    /**
     * @brief 建構函數
     * @param position 攻擊中心位置
     * @param delay 警告倒數時間
     * @param width 矩形寬度
     * @param height 矩形高度
     * @param rotation 矩形旋轉角度（弧度）
     * @param sequenceNumber 攻擊序列編號
     */
    RectangleAttack(const glm::vec2& position, float delay,
                   float width = 200.0f, float height = 100.0f,
                   float rotation = 0.0f, int sequenceNumber = 0);

    /**
     * @brief 設置矩形攻擊的尺寸
     * @param width 寬度
     * @param height 高度
     */
    void SetSize(float width, float height) {
        m_Width = width;
        m_Height = height;
    }

    /**
     * @brief 設置矩形攻擊的旋轉角度
     * @param rotation 旋轉角度（弧度）
     */
    void SetRotation(float rotation) {
        m_Rotation = rotation;
    }

    /**
     * @brief 取得矩形攻擊的尺寸
     * @return 尺寸向量，x=寬度，y=高度
     */
    glm::vec2 GetSize() const {
        return {m_Width, m_Height};
    }

    /**
     * @brief 取得矩形攻擊的旋轉角度
     * @return 旋轉角度（弧度）
     */
    float GetRotation() const {
        return m_Rotation;
    }

    /**
     * @brief 設置矩形攻擊的顏色
     * @param color 顏色值
     */
    void SetColor(const Util::Color& color) {
        m_Color = color;
    }

protected:
    // 實現基類要求的方法
    void CreateWarningEffect() override;
    void CreateAttackEffect() override;
    bool CheckCollisionInternal(const std::shared_ptr<Character>& character) override;

private:
    float m_Width;                 // 矩形寬度
    float m_Height;                // 矩形高度
    float m_Rotation;              // 矩形旋轉角度（弧度）
    Util::Color m_Color;           // 攻擊效果顏色
    bool m_UseGlowEffect = true;   // 是否使用發光效果

    // 輔助方法：檢查點是否在旋轉後的矩形內
    bool IsPointInRectangle(const glm::vec2& point) const;
};

#endif // RECTANGLEATTACK_HPP