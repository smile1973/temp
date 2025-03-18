// include/LaserAttack.hpp
#ifndef LASERATTACK_HPP
#define LASERATTACK_HPP
#include "Attack.hpp"

class LaserAttack : public Attack {
public:
    enum class Direction {
        HORIZONTAL,  // 水平雷射
        VERTICAL,    // 垂直雷射
        DIAGONAL_TL_BR,  // 左上到右下對角線
        DIAGONAL_TR_BL   // 右上到左下對角線
    };

    // 建構函數
    LaserAttack(const glm::vec2& position, float delay, Direction direction,
                float width = 80.0f, float length = 2000.0f, int sequenceNumber = 0);

    // 設置雷射屬性
    void SetDirection(Direction direction);
    void SetWidth(float width) { m_Width = width; }
    void SetLength(float length) { m_Length = length; }

    // 獲取雷射屬性
    Direction GetDirection() const { return m_Direction; }
    float GetWidth() const { return m_Width; }
    float GetLength() const { return m_Length; }

protected:
    // 實現Attack基類要求的方法
    void CreateWarningEffect() override;
    void CreateAttackEffect() override;
    bool CheckCollisionInternal(const std::shared_ptr<Character>& character) override;

    // 根據方向計算雷射的旋轉角度
    float CalculateRotationAngle() const;

    // 根據方向計算碰撞區域
    bool IsPointInLaser(const glm::vec2& point) const;

private:
    Direction m_Direction;
    float m_Width;     // 雷射寬度
    float m_Length;    // 雷射長度
    float m_RotationAngle = 0.0f;  // 雷射旋轉角度（弧度）
};

#endif //LASERATTACK_HPP
