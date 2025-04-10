#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Util/GameObject.hpp"

class Object : public Util::GameObject {
public:
    explicit Object(const std::string& ImagePath);

    Object(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(const Object&) = delete;
    Object& operator=(Object&&) = delete;

    [[nodiscard]] const std::string& GetImagePathSet() const { return m_ImagePath; }
    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool GetVisibility() const { return m_Visible; }

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }
    void SetInversion() { m_Transform.scale.x *= -1; } // 設定左右反轉角色
    void SetImage(const std::string& ImagePath); // 設定角色圖片


    // 技能
    void Update();

    void MovePosition(const glm::vec2& Position, float totalTime = 0.0f);  //平移位置
    void MoveToPosition(const glm::vec2& targetPosition, float totalTime = 0.0f); //平移到某位置
    [[nodiscard]] glm::vec2& GetTargetPosition() { return m_TargetPosition; }

private:
    void ResetPosition() { m_Transform.translation = {0, 0}; }

    std::string m_ImagePath;

    bool m_IsMoving = false;
    float m_DistanceTraveled = 0.0f;
    float m_TotalTime = 0.0f;
    glm::vec2 m_TargetPosition = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_MoveSpeed = glm::vec2(0.0f, 0.0f);
};


#endif //OBJECT_HPP
