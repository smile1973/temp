// include/Attack/CircleAttack.hpp
#ifndef CIRCLEATTACK_HPP
#define CIRCLEATTACK_HPP

#include "Attack/Attack.hpp"

/**
 * @class CircleAttack
 * @brief 圓形攻擊實現，用於創建圓形的攻擊區域
 */
class CircleAttack : public Attack {
public:
    /**
     * @brief 建構函數
     * @param position 攻擊中心位置
     * @param delay 警告倒數時間
     * @param radius 攻擊圓形半徑
     * @param sequenceNumber 攻擊序列編號
     */
    CircleAttack(const glm::vec2& position, float delay, float radius = 100.0f, int sequenceNumber = 0);

    /**
     * @brief 設置圓形攻擊的半徑
     * @param radius 半徑值（像素）
     */
    void SetRadius(float radius) { m_Radius = radius; }

    /**
     * @brief 取得圓形攻擊的半徑
     * @return 半徑值
     */
    float GetRadius() const { return m_Radius; }

    /**
     * @brief 設置圓形攻擊的顏色
     * @param color 顏色值
     */
    void SetColor(const Util::Color& color) { m_Color = color; }

protected:
    // 實現基類要求的方法
    void CreateWarningEffect() override;
    void CreateAttackEffect() override;
    bool CheckCollisionInternal(const std::shared_ptr<Character>& character) override;

private:
    float m_Radius;                // 圓形攻擊半徑
    Util::Color m_Color;           // 攻擊效果顏色
    bool m_UseGlowEffect = true;   // 是否使用發光效果
};

#endif // CIRCLEATTACK_HPP