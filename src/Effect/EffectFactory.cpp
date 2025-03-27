#include "Effect/EffectFactory.hpp"
#include "Util/Logger.hpp"
#include "Effect/Shape/RectangleShape.hpp"

namespace Effect {

    std::shared_ptr<CompositeEffect> EffectFactory::CreateEffect(EffectType type) {
        std::shared_ptr<CompositeEffect> effect;

        switch (type) {
            case EffectType::SKILL_Z: {
                // Z技能：實心圓，內部半透明，邊緣加深，不會移動
                auto circleShape = std::make_shared<Shape::CircleShape>(0.4f, 1.0f);
                circleShape->SetColor(Util::Color(1.0f, 0.8f, 0.7f, 0.3f)); // 半透明白色
                circleShape->SetSize({500, 500});
                effect = std::make_shared<CompositeEffect>(circleShape);
                effect->SetFillModifier(Modifier::FillModifier(Modifier::FillType::SOLID));
                effect->SetEdgeModifier(Modifier::EdgeModifier(Modifier::EdgeType::GLOW, 0.02f, Util::Color(1.0f, 0.0f, 1.0f, 1.0f)));
                break;
            }

            case EffectType::SKILL_X: {
                // X技能：空心圓，邊緣發光，有尾跡效果，會移動
                auto circleShape = std::make_shared<Shape::CircleShape>(0.4f, 2.0f);
                circleShape->SetColor(Util::Color(1.0f, 0.8f, 0.7f, 0.1f));
                circleShape->SetSize({150, 150});

                effect = std::make_shared<CompositeEffect>(circleShape);
                effect->SetFillModifier(Modifier::FillModifier(Modifier::FillType::HOLLOW, 0.02f));
                effect->SetEdgeModifier(Modifier::EdgeModifier(Modifier::EdgeType::GLOW, 0.02f, Util::Color(1.0f, 0.0f, 1.0f, 1.0f)));
                effect->SetMovementModifier(Modifier::MovementModifier(true, 600.0f, 800.0f, {1.0f, 0.0f}));
                effect->SetAnimationModifier(Modifier::AnimationModifier(Modifier::AnimationType::TRAIL, 1.0f, 2.0f));
                break;
            }

            case EffectType::SKILL_C: {
                // C技能：實心橢圓，內部半透明，邊緣加深，不會移動
                auto ellipseShape = std::make_shared<Shape::EllipseShape>(glm::vec2(0.4f, 0.05f), 1.0f);
                ellipseShape->SetColor(Util::Color(1.0f, 1.0f, 1.0f, 0.05f));
                ellipseShape->SetSize({700, 700});
                effect = std::make_shared<CompositeEffect>(ellipseShape);
                effect->SetFillModifier(Modifier::FillModifier(Modifier::FillType::HOLLOW, 0.01f));
                effect->SetEdgeModifier(Modifier::EdgeModifier(Modifier::EdgeType::GLOW, 0.03f, Util::Color(1.0f, 1.0f, 1.0f, 0.7f)));
                break;
            }

            case EffectType::SKILL_V: {
                // V技能：實心圓，內部半透明，邊緣發光，有波紋效果，不會移動
                auto circleShape = std::make_shared<Shape::CircleShape>(0.4f, 1.5f);
                circleShape->SetColor(Util::Color(0.9f, 0.9f, 0.9f, 0.05f)); // 半透明藍色

                effect = std::make_shared<CompositeEffect>(circleShape);
                effect->SetFillModifier(Modifier::FillModifier(Modifier::FillType::SOLID));
                effect->SetEdgeModifier(Modifier::EdgeModifier(Modifier::EdgeType::GLOW, 0.05f, Util::Color(1.0f, 0.0f, 1.0f, 1.0f)));
                effect->SetAnimationModifier(Modifier::AnimationModifier(Modifier::AnimationType::RIPPLE, 1.0f, 1.0f));
                break;
            }

            case EffectType::ENEMY_ATTACK_1: {
                // 敵人攻擊1：實心圓，邊緣加深，會移動
                auto circleShape = std::make_shared<Shape::CircleShape>(0.3f, 3.5f);
                circleShape->SetColor(Util::Color(1.0f, 0.0f, 0.0f, 0.5f)); // 半透明紅色

                effect = std::make_shared<CompositeEffect>(circleShape);
                effect->SetFillModifier(Modifier::FillModifier(Modifier::FillType::SOLID));
                effect->SetEdgeModifier(Modifier::EdgeModifier(Modifier::EdgeType::DARK, 0.03f));
                effect->SetMovementModifier(Modifier::MovementModifier(true, 250.0f, 400.0f, {0.0f, -1.0f}));
                break;
            }

            case EffectType::ENEMY_ATTACK_2: {
                // 敵人攻擊2：空心圓，邊緣發光，不會移動
                auto circleShape = std::make_shared<Shape::CircleShape>(0.35f, 1.0f);
                circleShape->SetColor(Util::Color(1.0f, 0.2f, 0.0f, 0.7f)); // 半透明橙紅色

                effect = std::make_shared<CompositeEffect>(circleShape);
                effect->SetFillModifier(Modifier::FillModifier(Modifier::FillType::HOLLOW, 0.04f));
                effect->SetEdgeModifier(Modifier::EdgeModifier(Modifier::EdgeType::GLOW, 0.06f, Util::Color(1.0f, 0.5f, 0.0f, 1.0f)));
                break;
            }

            case EffectType::ENEMY_ATTACK_3: {
                // 敵人攻擊3：實心圓，不會移動
                auto circleShape = std::make_shared<Shape::CircleShape>(0.3f, 0.5f);
                circleShape->SetColor(Util::Color(0.8f, 0.0f, 0.0f, 0.4f)); // 半透明紅色

                effect = std::make_shared<CompositeEffect>(circleShape);
                effect->SetFillModifier(Modifier::FillModifier(Modifier::FillType::SOLID));
                break;
            }

            case EffectType::RECT_LASER: {
                // 矩形雷射：實心矩形，邊緣發光
                auto rectangleShape = std::make_shared<Shape::RectangleShape>(
                    glm::vec2(1.0f, 0.1f), // 寬高比例
                    0.0f,                  // 粗細 (實心)
                    0.0f,                  // 初始旋轉角度
                    2.0f,                  // 持續時間
                    false,                 // 不啟用自動旋轉
                    0.0f                   // 旋轉速度
                );
                rectangleShape->SetColor(Util::Color(1.0f, 0.7f, 0.4f, 0.3f));
                rectangleShape->SetSize({2560, 2560}); // 效果尺寸

                effect = std::make_shared<CompositeEffect>(rectangleShape);
                effect->SetEdgeModifier(Modifier::EdgeModifier(Modifier::EdgeType::GLOW, 0.01f, Util::Color(1.0f, 0.0f, 1.0f, 0.9f)));
                // effect->SetMovementModifier(Modifier::MovementModifier(true, 800.0f, 1200.0f, {1.0f, 0.0f}));
                break;
            }

            case EffectType::RECT_BEAM: {
                // 矩形光束：空心矩形，邊緣發光，自動旋轉
                auto rectangleShape = std::make_shared<Shape::RectangleShape>(
                    glm::vec2(1.0f, 0.05f), // 寬高比例
                    0.0,                     // 粗細 (實心) 空心放 width
                    0.0f,                    // 初始旋轉角度
                    5.0f,                    // 持續時間
                    true,                    // 啟用自動旋轉
                    0.3f                     // 旋轉速度 (弧度/秒)
                );
                rectangleShape->SetColor(Util::Color(1.0f, 0.7f, 0.4f, 0.3f));
                rectangleShape->SetSize({1600, 1600}); // 效果尺寸

                effect = std::make_shared<CompositeEffect>(rectangleShape);
                effect->SetFillModifier(Modifier::FillModifier(Modifier::FillType::SOLID));
                effect->SetEdgeModifier(Modifier::EdgeModifier(Modifier::EdgeType::GLOW, 0.02f, Util::Color(1.0f, 0.0f, 1.0f, 0.9f)));
                break;
            }
                
            default:
                LOG_ERROR("Unknown effect type requested from EffectFactory");
                // Provide a default effect
                auto defaultShape = std::make_shared<Shape::CircleShape>(0.3f, 1.0f);
                defaultShape->SetColor(Util::Color(255, 0, 255, 128)); // Semi-transparent magenta
                effect = std::make_shared<CompositeEffect>(defaultShape);
                break;
        }
        
        return effect;
    }

    std::shared_ptr<CompositeEffect> EffectFactory::CreateCustomEffect(
        bool isCircle,
        const Modifier::FillType& fillType,
        const Modifier::EdgeType& edgeType,
        bool isMoving,
        const Modifier::AnimationType& animType
    ) {
        std::shared_ptr<Shape::BaseShape> baseShape;
        
        if (isCircle) {
            baseShape = std::make_shared<Shape::CircleShape>(0.4f, 1.0f);
        } else {
            baseShape = std::make_shared<Shape::EllipseShape>(glm::vec2(0.5f, 0.3f), 1.0f);
        }
        
        auto effect = std::make_shared<CompositeEffect>(baseShape);
        
        // Set fill modifier
        effect->SetFillModifier(Modifier::FillModifier(fillType, 0.03f));

        // Set edge modifier
        effect->SetEdgeModifier(Modifier::EdgeModifier(edgeType, 0.05f, Util::Color(255, 255, 255, 255)));

        // Set movement modifier
        if (isMoving) {
            effect->SetMovementModifier(Modifier::MovementModifier(true, 250.0f, 400.0f, {1.0f, 0.0f}));
        }

        // Set animation modifier
        if (animType != Modifier::AnimationType::NONE) {
            effect->SetAnimationModifier(Modifier::AnimationModifier(animType, 1.0f, 1.0f));
        }
        
        return effect;
    }

} // namespace Effect