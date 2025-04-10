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

    /**
     * @brief 設置移動參數
     * @param direction 移動方向
     * @param speed 移動速度
     * @param distance 總移動距離
     */
    void SetMovementParams(const glm::vec2& direction, float speed, float distance) {
        m_Direction = direction;
        m_Speed = speed;
        m_Distance = distance;
        m_IsMoving = true;
    }
    void CleanupVisuals() override;

protected:
    // 實現基類要求的方法
    void CreateWarningEffect() override;
    void CreateAttackEffect() override;
    bool CheckCollisionInternal(const std::shared_ptr<Character>& character) override;
    void SyncWithEffect() override;
    void OnCountdownStart() override;
    void OnAttackStart() override;

private:
    float m_Radius;                // 圓形攻擊半徑
    Util::Color m_Color;           // 攻擊效果顏色
    bool m_UseGlowEffect = true;   // 是否使用發光效果

    bool m_IsMoving = false;       // 是否啟用移動
    glm::vec2 m_Direction = {1.0f, 0.0f}; // 移動方向
    float m_Speed = 200.0f;        // 移動速度
    float m_Distance = 800.0f;     // 移動距離

    // 方向箭頭圖片
    static std::shared_ptr<Util::Image> s_ArrowImage;
    std::shared_ptr<Util::GameObject> m_DirectionIndicator;

    void CreateDirectionIndicator();
    float CalculateDirectionAngle() const;
};

#endif // CIRCLEATTACK_HPP