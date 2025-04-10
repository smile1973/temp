#include "Character.hpp"
#include "Util/Renderer.hpp"
#include "Util/Logger.hpp"

bool Character::IfCollides(const std::shared_ptr<Character>& other, const float Distance) const{
    if (!other) return false;
    // 取得當前角色和另一個角色的位置
    glm::vec2 pos1 = this->GetPosition();
    glm::vec2 pos2 = other->GetPosition();
    float size = Distance;
    // 簡單的 AABB (Axis-Aligned Bounding Box) 碰撞檢測
    bool isColliding = (abs(pos1.x - pos2.x) < size) && (abs(pos1.y - pos2.y) < size);
    return isColliding;
}

bool Character::IfCirclesCollide(const std::shared_ptr<Character>& other, const float Distance) const {
    if (!other) return false;
    // 判斷是否碰撞
    return glm::distance(this->GetPosition(), other->GetPosition()) < Distance ;
}