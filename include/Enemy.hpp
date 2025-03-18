#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Character.hpp"
#include "Util/Renderer.hpp"
#include "Util/Time.hpp"
#include "Util/Logger.hpp"

#include <glm/glm.hpp>
#include <iostream>

// Enemy 類別，繼承自 Character，代表遊戲中的敵人角色
class Enemy : public Character {
public:
    // 構造函數，初始化敵人的血量與影像集
    Enemy(float health, const std::vector<std::string>& ImageSet);

    // 檢查敵人是否仍然存活
    [[nodiscard]] bool IfAlive() const{return m_Health > 0.0f;};

    // 讓敵人受到傷害，減少生命值
    void TakeDamage(float damage);

    // 更改敵人血量，默認原血量
    void SetHealth(float Health =-1.0f);

    void MovePosition(const glm::vec2& Position) { m_Transform.translation += Position; }
    // void MovePosition(const glm::vec2& targetPosition, float totalTime=2.0f);

    // 繪製敵人的血條
    void DrawHealthBar(const glm::vec2& position) const;

private:
    // 初始化著色程序（Shader Program）
    static void InitProgram();

    // 初始化頂點陣列（Vertex Array Object）
    static void InitVertexArray();

    // 初始化 Uniform 變數（著色器中的全域變數）
    void InitUniforms();

    // 靜態成員變數：共享的著色程序
    static std::unique_ptr<Core::Program> s_Program;

    // 靜態成員變數：共享的頂點數據
    static std::unique_ptr<Core::VertexArray> s_VertexArray;

    // 敵人的當前血量與最大血量
    float m_Health;
    float m_MaxHealth;

    // Uniform 變數位置（顏色與血條寬度）
    GLint m_ColorLocation;
    GLint m_WidthLocation;
};

#endif // ENEMY_HPP
