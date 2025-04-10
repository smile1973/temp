#ifndef CORNERBULLETATTACK_HPP
#define CORNERBULLETATTACK_HPP

#include "Attack/Attack.hpp"
#include <vector>
#include <random>

class CornerBulletAttack : public Attack {
public:
    // 構造函數
    CornerBulletAttack(float delay, int bulletCount = 3, int sequenceNumber = 0);

    // 設置攻擊參數
    void SetBulletSpeed(float speed) { m_BulletSpeed = speed; }
    void SetBulletRadius(float radius) { m_BulletRadius = radius; }
    void SetBulletCount(int count) { m_BulletCount = count; }

    // 添加一個新的彈道
    void AddBulletPath(const glm::vec2& startPosition, float angle);

protected:
    // 實現父類的虛函數
    void CreateWarningEffect() override;
    void CreateAttackEffect() override;
    bool CheckCollisionInternal(const std::shared_ptr<Character>& character) override;
    void SyncWithEffect() override;

private:
    // 生成隨機子彈角度
    std::vector<float> GenerateRandomAngles(float base);

    // 子彈路徑結構
    struct BulletPath {
        glm::vec2 startPosition;      // 發射起點
        glm::vec2 currentPosition;    // 當前位置 (用於碰撞檢測)
        float angle;                  // 發射角度（弧度）
        std::shared_ptr<Effect::CompositeEffect> warningEffect;  // 軌跡警告效果
        std::shared_ptr<Effect::CompositeEffect> bulletEffect;   // 子彈特效
        bool isActive = false;        // 子彈是否活躍
    };

    // 成員變量
    std::vector<BulletPath> m_BulletPaths;    // 所有子彈路徑
    float m_BulletSpeed = 350.0f;            // 子彈移動速度
    float m_BulletRadius = 30.0f;            // 子彈半徑
    int m_BulletCount = 3;                   // 每個角落的子彈數量
    std::default_random_engine m_RandomEngine;// 隨機數生成器
};

#endif // CORNERBULLETATTACK_HPP
