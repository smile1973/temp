#include "Effect/EffectManager.hpp"
#include "Util/TransformUtils.hpp"
#include "Util/Logger.hpp"

namespace Effect {

    void EffectManager::Initialize(size_t initialPoolSize) {
        // Make sure we initialize in specific order to ensure shader loading works properly
        EffectType initOrder[] = {
            EffectType::SKILL_Z,  // May contain basic shader initialization
            EffectType::SKILL_X,
            EffectType::SKILL_C,
            EffectType::SKILL_V,
            EffectType::ENEMY_ATTACK_1,
            EffectType::ENEMY_ATTACK_2,
            EffectType::ENEMY_ATTACK_3,
            EffectType::RECT_LASER,
            EffectType::RECT_BEAM,
        };
        
        for (auto type : initOrder) {
            for (size_t j = 0; j < initialPoolSize; ++j) {
                auto effect = EffectFactory::GetInstance().CreateEffect(type);

                // Store the effect type in the effect for future reference
                effect->GetBaseShape()->SetUserData(static_cast<int>(type));

                m_InactiveEffects[type].push(effect);
            }
        }
        LOG_INFO("EffectManager initialized with {} effects per type", initialPoolSize);
    }

    std::shared_ptr<CompositeEffect> EffectManager::GetEffect(EffectType type) {
        std::shared_ptr<CompositeEffect> effect;

        // Try to get effect from the pool
        if (!m_InactiveEffects[type].empty()) {
            effect = m_InactiveEffects[type].front();
            m_InactiveEffects[type].pop();
            LOG_DEBUG("Retrieved effect from pool, type: {}", static_cast<int>(type));
        } else {
            // Create new effect
            effect = EffectFactory::GetInstance().CreateEffect(type);

            // Store the effect type in the effect for future reference
            effect->GetBaseShape()->SetUserData(static_cast<int>(type));

            LOG_DEBUG("Created new effect, type: {}", static_cast<int>(type));
        }

        // Add to active effects list
        m_ActiveEffects.push_back(effect);
        return effect;
    }

    void EffectManager::Draw() {
        // This method is called by GameObject's Draw method
        for (auto& effect : m_ActiveEffects) {
            if (effect->IsActive()) {
                // Get transformation matrix
                auto data = Util::ConvertToUniformBufferData(
                    Util::Transform{effect->GetPosition(), 0, {1, 1}},
                    effect->GetSize(),
                    m_ZIndex
                );

                // Draw effect
                effect->Draw(data);
            }
        }
    }

    std::shared_ptr<CompositeEffect> EffectManager::PlayEffect(
        EffectType type,
        const glm::vec2& position,
        float zIndex,
        float duration
    ) {
        // Get effect
        auto effect = GetEffect(type);

        // Set duration
        effect->SetDuration(duration);

        // Play effect
        effect->Play(position, zIndex);

        return effect;
    }

    void EffectManager::Update(float deltaTime) {
        // Update all active effects
        for (auto it = m_ActiveEffects.begin(); it != m_ActiveEffects.end();) {
            auto effect = *it;
            effect->Update(deltaTime);

            if (effect->IsFinished()) {
                // Reset effect
                effect->Reset();

                // Get the effect type from user data
                int typeValue = effect->GetBaseShape()->GetUserData();
                EffectType type = static_cast<EffectType>(typeValue);

                // Return effect to its proper pool
                m_InactiveEffects[type].push(effect);
                LOG_DEBUG("Returned effect to pool, type: {}", typeValue);

                // Remove from active list
                it = m_ActiveEffects.erase(it);
            } else {
                ++it;
            }
        }
    }
}