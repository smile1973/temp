#include "Attack/CornerBulletAttack.hpp"
#include "Util/Logger.hpp"
#include "Effect/EffectManager.hpp"
#include <cmath>

CornerBulletAttack::CornerBulletAttack(float delay, int bulletCount, int sequenceNumber)
    : Attack(glm::vec2(0, 0), delay, sequenceNumber),  // 中心點不重要，因為子彈從四個角落發射
      m_BulletCount(bulletCount) {
    
    // 初始化隨機數生成器
    std::random_device rd;
    m_RandomEngine.seed(rd());
}

std::vector<float> CornerBulletAttack::GenerateRandomAngles(float baseAngle) {
    std::vector<float> angles;

    // 固定偏移角
    // 將角度轉換為弧度 (π/180 * 角度)
    std::vector<float> fixedOffsets1 = {
        M_PI / 180.0f * 35.0f,
        M_PI / 180.0f * 55.0f,
        M_PI / 180.0f * 75.0f
    };
    std::vector<float> fixedOffsets2 = {
        M_PI / 180.0f * 15.0f,
        M_PI / 180.0f * 35.0f,
        M_PI / 180.0f * 55.0f
    };

    // 隨機偏移 ±5度
    float maxDeviation = M_PI / 180.0f * 5.0f;  // 5度轉換為弧度
    std::uniform_real_distribution<float> distribution(-maxDeviation, maxDeviation);


    // 生成每個子彈的角度（基準角度 + 固定偏移 + 隨機偏移）
    std::vector<float> use = (baseAngle == 0.0f || baseAngle == M_PI) ? fixedOffsets1 : fixedOffsets2;

    for (int i = 0; i < m_BulletCount; ++i) {
        float angle = baseAngle + use[i] + distribution(m_RandomEngine);
        angles.push_back(angle);
        LOG_DEBUG("Generated bullet angle: {:.2f} ({:.4f} rad)", angle * 180.0f / M_PI, angle);
    }
    return angles;
}

void CornerBulletAttack::AddBulletPath(const glm::vec2& startPosition, float angle) {
    BulletPath path;
    path.startPosition = startPosition;
    path.angle = angle;
    m_BulletPaths.push_back(path);
}

void CornerBulletAttack::CreateWarningEffect() {
    m_BulletPaths.clear();

    glm::vec2 topLeft(-642.0f, 362.0f);      // 左上角
    glm::vec2 topRight(642.0f, 362.0f);      // 右上角
    glm::vec2 bottomLeft(-642.0f, -362.0f);  // 左下角
    glm::vec2 bottomRight(642.0f, -362.0f);  // 右下角

    // 左上角 (向0~90) 大
    auto tlAngles = GenerateRandomAngles(0.0f);
    for (float angle : tlAngles) {
        AddBulletPath(topLeft, angle);
    }
    
    // 右上角 (向90~180) 小
    auto trAngles = GenerateRandomAngles(2.0f * M_PI / 4.0f);
    for (float angle : trAngles) {
        AddBulletPath(topRight, angle);
    }

    // 右下角 (向180~270) 大
    auto brAngles = GenerateRandomAngles(4.0f * M_PI / 4.0f);
    for (float angle : brAngles) {
        AddBulletPath(bottomRight, angle);
    }

    // 左下角 (向270~360) 小
    auto blAngles = GenerateRandomAngles(6.0f * M_PI / 4.0f);
    for (float angle : blAngles) {
        AddBulletPath(bottomLeft, angle);
    }
    

    
    // 為每個彈道創建警告效果
    for (auto& path : m_BulletPaths) {
        // 計算彈道終點
        float distance = 3000.0f;  // 足夠長的距離
        glm::vec2 endPosition = path.startPosition +
            glm::vec2(cos(path.angle), sin(path.angle)) * distance;

        // 長度和旋轉角度
        float length = glm::distance(path.startPosition, endPosition);
        float width = m_BulletRadius * 1.5f;

        // 創建矩形警告效果
        auto warningEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_BEAM);
        if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(warningEffect->GetBaseShape())) {
            rectangleShape->SetDimensions(glm::vec2(1.0f, width / length));
            rectangleShape->SetRotation(path.angle);
            rectangleShape->SetSize({length,length});
            rectangleShape->SetColor(Util::Color(1.0f, 0.0f, 0.0f, 0.3f));

            rectangleShape->SetAutoRotation(false);
        }
        warningEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));
        warningEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(Effect::Modifier::EdgeType::DARK, 0.001, Util::Color(0.9, 0.1, 0.1, 0.7)));
        warningEffect->SetDuration(m_Delay + 0.5f);
        // LOG_DEBUG("path.angle: {}, startPos : {}, endpos: {}, length : {}, width: {}", path.angle, path.startPosition, endPosition, length, width);

        warningEffect->Play(path.startPosition, m_ZIndex + 0.1f);
        path.warningEffect = warningEffect;
    }
    
    // LOG_DEBUG("Created warning effects for {} bullet paths", m_BulletPaths.size());
}

void CornerBulletAttack::CreateAttackEffect() {
    for (auto& path : m_BulletPaths) {
        auto bulletEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::ENEMY_ATTACK_1);
        
        if (auto circleShape = std::dynamic_pointer_cast<Effect::Shape::CircleShape>(bulletEffect->GetBaseShape())) {
            float normalizedRadius = 0.35f;
            circleShape->SetRadius(normalizedRadius);

            float visualSize = m_BulletRadius * 2.5f;
            circleShape->SetSize({visualSize, visualSize});
            circleShape->SetColor(Util::Color(1.0f, 0.0f, 0.0f, 0.7f));
        }
        
        // 設置填充與邊緣
        bulletEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));
        bulletEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(
            Effect::Modifier::EdgeType::GLOW, 
            0.01f, 
            Util::Color(1.0f, 0.0f, 0.0f, 0.9f)
        ));
        
        // 設置移動
        glm::vec2 direction(cos(2.0f * M_PI - path.angle), sin(2.0f * M_PI - path.angle));
        bulletEffect->SetMovementModifier(Effect::Modifier::MovementModifier(
            true,                // 啟用移動
            m_BulletSpeed,       // 移動速度
            1500.0f,             // 移動距離
            direction            // 移動方向
        ));
        
        // 設置持續時間
        bulletEffect->SetDuration(5.0f);  // 足夠長的時間確保子彈飛出屏幕

        bulletEffect->Play(path.startPosition, m_ZIndex + 1.0f);
        path.bulletEffect = bulletEffect;
    }
    
    // LOG_DEBUG("Created attack effects for {} bullet paths", m_BulletPaths.size());
}

bool CornerBulletAttack::CheckCollisionInternal(const std::shared_ptr<Character>& character) {
    if (!character) return false;
    SyncWithEffect();
    glm::vec2 characterPos = character->GetPosition();
    for (const auto& path : m_BulletPaths) {
        if (path.bulletEffect && path.bulletEffect->IsActive()) {
            float distance = glm::length(characterPos - path.currentPosition);
            if (distance <= m_BulletRadius) {
                return true;
            }
        }
    }
    return false;
}

void CornerBulletAttack::SyncWithEffect() {
    for (auto& path : m_BulletPaths) {
        if (path.bulletEffect && path.bulletEffect->IsActive()) {
            path.currentPosition = path.bulletEffect->GetPosition();
        }
    }
}