#ifndef EFFECT_FACTORY_HPP
#define EFFECT_FACTORY_HPP

#include "Effect/CompositeEffect.hpp"

namespace Effect {

    // 預定義特效類型
    enum class EffectType {
        // 角色技能特效
        SKILL_Z,        // Z技能 : 實心圓，內部半透明，邊緣加深，不會移動
        SKILL_X,        // X技能 : 空心圓，邊緣發光，有尾跡效果，會移動
        SKILL_C,        // C技能 : 實心橢圓，內部半透明，邊緣加深，不會移動
        SKILL_V,        // V技能 : 實心圓，內部半透明，邊緣發光，有波紋效果，不會移動

        // 敵人攻擊特效
        ENEMY_ATTACK_1, // 實心圓，邊緣加深，會移動
        ENEMY_ATTACK_2, // 空心圓，邊緣發光，不會移動
        ENEMY_ATTACK_3, // 實心圓，不會移動

        // 矩形特效
        RECT_LASER,     // 矩形雷射，實心，邊緣發光，會移動
        RECT_BEAM,      // 矩形光束，空心，邊緣加深，可旋轉
    };

    class EffectFactory {
    public:
        static EffectFactory& GetInstance() {
            static EffectFactory instance;
            return instance;
        }

        // 創建預定義特效
        static std::shared_ptr<CompositeEffect> CreateEffect(EffectType type);

        // 創建自定義組合特效
        std::shared_ptr<CompositeEffect> CreateCustomEffect(
            bool isCircle = true,
            const Modifier::FillType& fillType = Modifier::FillType::SOLID,
            const Modifier::EdgeType& edgeType = Modifier::EdgeType::NONE,
            bool isMoving = false,
            const Modifier::AnimationType& animType = Modifier::AnimationType::NONE
        );

    private:
        EffectFactory() = default;
    };

} // namespace Effect

#endif // EFFECT_FACTORY_HPP