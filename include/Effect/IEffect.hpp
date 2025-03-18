#ifndef IEFFECT_HPP
#define IEFFECT_HPP

#include "pch.hpp"
#include "Core/Drawable.hpp"
#include "Util/Transform.hpp"

namespace Effect {
    // 特效基類
    class IEffect : public Core::Drawable {
    public:
        enum class State {
            INACTIVE,   // 未激活（可用於對象池回收）
            ACTIVE,     // 活躍中
            FINISHED    // 已結束（可以被回收）
        };

        IEffect() = default;
        virtual ~IEffect() = default;

        // 生命週期管理
        virtual void Update(float deltaTime) = 0;
        virtual void Play(const glm::vec2& position, float zIndex = 0.0f) = 0;
        virtual void Reset() = 0;

        // 狀態訪問
        State GetState() const { return m_State; }
        bool IsFinished() const { return m_State == State::FINISHED; }
        bool IsActive() const { return m_State == State::ACTIVE; }

        // 獲取和設置位置
        const glm::vec2& GetPosition() const { return m_Transform.translation; }
        void SetPosition(const glm::vec2& position) { m_Transform.translation = position; }
        
        // 獲取特效持續時間
        float GetDuration() const { return m_Duration; }
        void SetDuration(float duration) { m_Duration = duration; }

    protected:
        State m_State = State::INACTIVE;
        Util::Transform m_Transform;
        float m_Duration = 1.0f;         // 特效持續時間（秒）
        float m_ElapsedTime = 0.0f;      // 已經過的時間（秒）
        float m_ZIndex = 0.0f;           // Z-index 用於繪製順序
    };
}

#endif // IEFFECT_HPP