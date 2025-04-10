// include/Attack/RectangleAttack.hpp
#ifndef RECTANGLEATTACK_HPP
#define RECTANGLEATTACK_HPP

#include "Attack/Attack.hpp"

class RectangleAttack : public Attack {
public:
    enum class Direction {
        HORIZONTAL,  // 水平雷射
        VERTICAL,    // 垂直雷射
        DIAGONAL_TL_BR,  // 左上到右下對角線
        DIAGONAL_TR_BL,  // 右上到左下對角線
        CUSTOM
    };
    RectangleAttack(const glm::vec2& position, float delay,
                   float width = 200.0f, float height = 100.0f,
                   float rotation = 0.0f, int sequenceNumber = 0);

    RectangleAttack(const glm::vec2& position, float delay, Direction direction,
            float width = 80.0f, float length = 2000.0f, int sequenceNumber = 0);

    glm::vec2 GetSize() const { return {m_Width, m_Height}; }
    void SetSize(float width, float height) { m_Width = width; m_Height = height;}

    float GetRotation() const { return m_Rotation; }
    void SetRotation(float rotation);

    void SetColor(const Util::Color& color) { m_Color = color; }

    void SetAutoRotation(bool enable, float speed = 0.5f) {
        m_AutoRotate = enable;
        m_RotationSpeed = speed;
    }
    bool IsAutoRotating() const { return m_AutoRotate; }
    float GetRotationSpeed() const { return m_RotationSpeed; }
    void CleanupVisuals() override;

protected:
    void CreateWarningEffect() override;
    void CreateAttackEffect() override;
    bool CheckCollisionInternal(const std::shared_ptr<Character>& character) override;
    void SyncWithEffect() override;
    void OnCountdownStart() override;
    void OnCountdownUpdate(float deltaTime) override;
    void OnAttackStart() override;


private:
    [[nodiscard]] float CalculateRotationAngle() const;
    [[nodiscard]] bool IsPointInPolygon(const glm::vec2& point, const glm::vec2* vertices, int vertexCount) const;
    [[nodiscard]] bool IsPointInRectangle(const glm::vec2& point) const;

    float m_Width;                 // 矩形寬度
    float m_Height;                // 矩形高度
    float m_Rotation;              // 矩形旋轉角度（弧度）
    Util::Color m_Color;           // 攻擊效果顏色
    bool m_UseGlowEffect = true;   // 是否使用發光效果
    Direction m_Direction;

    bool m_AutoRotate = false;       // 是否啟用自動旋轉
    float m_RotationSpeed = 0.5f;    // 旋轉速度（弧度/秒）

    static std::shared_ptr<Util::Image> s_ClockwiseImage;
    static std::shared_ptr<Util::Image> s_CounterClockwiseImage;
    std::shared_ptr<Util::GameObject> m_DirectionIndicator;

    void CreateDirectionIndicator();
};

#endif // RECTANGLEATTACK_HPP