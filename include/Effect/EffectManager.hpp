#ifndef EFFECT_MANAGER_HPP
#define EFFECT_MANAGER_HPP

#include <queue>
#include "Effect/CompositeEffect.hpp"
#include "Effect/EffectFactory.hpp"
#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"
#include "Util/TransformUtils.hpp"

namespace Effect {
    class EffectManager : public Util::GameObject {
    public:
        static EffectManager& GetInstance() {
            static EffectManager instance;
            return instance;
        }

        // 初始化對象池
        void Initialize(size_t initialPoolSize = 10);

        // 獲取或創建特效
        std::shared_ptr<CompositeEffect> GetEffect(EffectType type);

        // 更新所有特效
        void Update(float deltaTime);

        // 繪製所有特效
        void Draw() override;

        // 直接播放特效
        std::shared_ptr<CompositeEffect> PlayEffect(
            EffectType type,
            const glm::vec2& position,
            float zIndex = 0.0f,
            float duration = 1.0f
        );

        // 取得活躍特效數量
        size_t GetActiveEffectsCount() const { return m_ActiveEffects.size(); }

    private:
        EffectManager() : Util::GameObject(nullptr, 30.0f) {}

        // 特效對象池，按類型分類
        std::unordered_map<EffectType, std::queue<std::shared_ptr<CompositeEffect>>> m_InactiveEffects;

        // 當前活躍的特效
        std::vector<std::shared_ptr<CompositeEffect>> m_ActiveEffects;
    };
}

#endif // EFFECT_MANAGER_HPP