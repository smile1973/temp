// src/Attack/Attack.cpp
#include "Attack/Attack.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "Effect/EffectFactory.hpp"
#include "Effect/EffectManager.hpp"

// 建構函數
Attack::Attack(const glm::vec2& position, float delay, int sequenceNumber)
    : Util::GameObject(nullptr, 20.0f), // Z索引設為20，確保攻擊效果在前景
      m_Position(position),
      m_Delay(delay),
      m_SequenceNumber(sequenceNumber){

    // 初始化設置
    m_Transform.translation = position;
    // 設置初始狀態為CREATED
    m_State = State::CREATED;

    // LOG_DEBUG("Attack created at position ({}, {}), delay: {}, sequence: {}",
    //          position.x, position.y, delay, sequenceNumber);
}

// 主要更新函數 - 根據當前狀態調用相應的處理函數
void Attack::Update(float deltaTime) {
    // 首次更新時，轉換到WARNING狀態
    if (m_IsFirstUpdate) {
        m_IsFirstUpdate = false;
        ChangeState(State::WARNING);
        return; // 讓下一幀再開始計時
    }

    m_ElapsedTime += deltaTime;

    switch (m_State) {
        case State::WARNING:
            OnWarningUpdate(deltaTime);

            // 警告階段結束時，轉換到倒數階段
            if (m_ElapsedTime >= 0.5f) { // 警告顯示0.5秒後開始倒數
                ChangeState(State::COUNTDOWN);
            }
            break;

        case State::COUNTDOWN:
            OnCountdownUpdate(deltaTime);

            // 更新時間條
            UpdateTimeBar(CalculateProgress());

            // 倒數結束後，轉換到攻擊階段
            if (m_ElapsedTime >= m_Delay) {
                ChangeState(State::ATTACKING);
            }
            break;

        case State::ATTACKING:
            OnAttackUpdate(deltaTime);

            // 攻擊階段結束後，轉換到完成階段
            if (m_ElapsedTime >= m_AttackDuration) {
                ChangeState(State::FINISHED);
            }
            break;

        case State::FINISHED:
            // 已結束的攻擊不做額外處理
            break;

        case State::CREATED:
            break;
    }
}

// 繪製函數 - GameObject的虛函數
void Attack::Draw() {
    if (m_State == State::CREATED) {
        return;
    }
    Util::GameObject::Draw();
}

// 設置攻擊位置
void Attack::SetPosition(const glm::vec2& position) {
    m_Position = position;
    m_Transform.translation = position;

    // 如果已經創建了特效，也要更新它們的位置
    if (m_WarningEffect) {
        m_WarningEffect->Play(position, m_ZIndex + 0.1f);
    }

    if (m_SequenceText) {
        m_SequenceText->SetText(std::to_string(m_SequenceNumber));
    }
}

// 設置序列號
void Attack::SetSequenceNumber(int number) {
    m_SequenceNumber = number;

    // 如果已經創建了序列文字，更新它
    if (m_SequenceText) {
        m_SequenceText->SetText(std::to_string(m_SequenceNumber));
    }
}

// 狀態轉換函數
void Attack::ChangeState(State newState) {
    // 如果狀態沒有變化，直接返回
    if (m_State == newState) return;

    // 保存舊狀態
    State oldState = m_State;
    m_State = newState;
    m_ElapsedTime = 0.0f; // 重置計時器

    // 調用對應的狀態開始處理函數
    switch (newState) {
        case State::WARNING:
            OnWarningStart();
            break;

        case State::COUNTDOWN:
            OnCountdownStart();
            break;

        case State::ATTACKING:
            OnAttackStart();
            break;

        case State::FINISHED:
            OnFinishedStart();
            break;

        case State::CREATED:
            // 不應該手動轉換為CREATED狀態
            LOG_ERROR("Attempt to change attack state to CREATED");
            break;
    }

    LOG_DEBUG("Attack state changed from {} to {}", static_cast<int>(oldState), static_cast<int>(newState));
}

// 警告階段開始
void Attack::OnWarningStart() {
    // LOG_DEBUG("OnWarningStart called, creating warning effect");
    CreateWarningEffect();

    // 創建序列號文字
    // if (m_SequenceNumber > 0) {
    //     // 創建序列號文字
    //     try {
    //         m_SequenceText = std::make_shared<Util::Text>(
    //             GA_RESOURCE_DIR "Font/Inkfree.ttf", 48,
    //             std::to_string(m_SequenceNumber),
    //             Util::Color::FromName(Util::Colors::WHITE)
    //         );
    //
    //         auto text = std::make_shared<Util::GameObject>(
    //             m_SequenceText,
    //             m_ZIndex + 0.3f,
    //             glm::vec2(0, 0),
    //             true
    //             );
    //         text->m_Transform.translation = m_Position;
    //         // 將文字添加為子物件
    //         m_SequenceTextObject = text;
    //         AddChild(text);
    //
    //         LOG_DEBUG("Sequence text created: {}", m_SequenceNumber);
    //     } catch (const std::exception& e) {
    //         LOG_ERROR("Failed to create sequence text: {}", e.what());
    //     }
    // }
}

// 警告階段更新
void Attack::OnWarningUpdate(float deltaTime) {
    // 預設實現暫時不做特殊處理
    (void)deltaTime;
}

// 倒數階段開始
void Attack::OnCountdownStart() {
    LOG_DEBUG("OnCountdownStart called, creating time bar");
    // 創建時間條
    try {
        CreateTimeBar();
        LOG_DEBUG("Time bar created successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("Error creating time bar: {}", e.what());
    }
}

// 倒數階段更新
void Attack::OnCountdownUpdate(float deltaTime) {
    (void)deltaTime;
}

// 攻擊階段開始
void Attack::OnAttackStart() {
    LOG_DEBUG("OnAttackStart called, creating attack effect");
    // 創建攻擊特效
    try {
        CreateAttackEffect();
        LOG_DEBUG("Attack effect created successfully");
    } catch (const std::exception& e) {
        LOG_ERROR("Error creating attack effect: {}", e.what());
    }

    // 移除警告特效和時間條
    if (m_WarningEffect) {
        // LOG_DEBUG("Removing warning effect at attack start");
        m_WarningEffect->Reset();
    }

    if (m_TimeBarEffect) {
        // LOG_DEBUG("Removing time bar at attack start");
        m_TimeBarEffect->Reset();
    }

    // 移除序列號文字
    if (m_SequenceTextObject) {
        RemoveChild(m_SequenceTextObject);
        m_SequenceTextObject = nullptr;
        m_SequenceText = nullptr;
    }
}

void Attack::OnAttackUpdate(float deltaTime) {
    SyncWithEffect();
    if (m_TargetCharacter) {
        CheckCollision(m_TargetCharacter);
    }

    // 檢查特效是否已結束，但攻擊仍在持續中
    if (m_AttackEffect && m_AttackEffect->IsFinished()) {
        CreateAttackEffect();
    }
}

// 完成階段開始
void Attack::OnFinishedStart() {
    // 清理所有特效
    if (m_AttackEffect) {
        m_AttackEffect->Reset();
    }

    LOG_DEBUG("Attack finished at position ({}, {})", m_Position.x, m_Position.y);
}

// 創建時間條
void Attack::CreateTimeBar() {
    // 使用矩形特效創建一個時間條
    auto rectangleEffect = Effect::EffectManager::GetInstance().GetEffect(Effect::EffectType::RECT_BEAM);

    // 設置時間條的屬性
    if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(rectangleEffect->GetBaseShape())) {
        rectangleShape->SetDimensions(glm::vec2(1.0f, 0.1f)); // 寬高比
        rectangleShape->SetSize({200.0, 200.0});
        rectangleShape->SetRotation(0.0f); // 不旋轉
        rectangleShape->SetAutoRotation(false); // 禁用自動旋
    }

    // 設置填充與邊緣效果
    rectangleEffect->SetFillModifier(Effect::Modifier::FillModifier(Effect::Modifier::FillType::SOLID));
    rectangleEffect->SetEdgeModifier(Effect::Modifier::EdgeModifier(
        Effect::Modifier::EdgeType::GLOW,
        0.005f,
        Util::Color::FromName(Util::Colors::WHITE)
    ));

    rectangleEffect->GetBaseShape()->SetColor(Util::Color(0.9, 0.9, 0.9, 0.5));

    // 設置位置 - 在警告效果下方
    glm::vec2 barPosition = m_Position;
    barPosition.y -= 50.0f; // 向下偏移

    // 播放時間條特效
    rectangleEffect->SetDuration(m_Delay);
    rectangleEffect->Play(barPosition, m_ZIndex + 1.0f);

    m_TimeBarEffect = rectangleEffect;
}

// 更新時間條
void Attack::UpdateTimeBar(float progress) {
    if (!m_TimeBarEffect) return;

    // 根據進度更新時間條的寬度和顏色
    if (auto rectangleShape = std::dynamic_pointer_cast<Effect::Shape::RectangleShape>(m_TimeBarEffect->GetBaseShape())) {
        // 更新寬度
        float width = 0.8f * (1.0f - progress);
        rectangleShape->SetDimensions(glm::vec2(width, 0.05f));
    }
}

// 計算倒數進度
float Attack::CalculateProgress() const {
    if (m_Delay <= 0.0f) return 1.0f;
    return m_ElapsedTime / m_Delay;
}

// 碰撞檢測
bool Attack::CheckCollision(const std::shared_ptr<Character>& character) {
    // 只檢查攻擊階段
    if (m_State != State::ATTACKING) return false;

    // 如果角色處於無敵狀態，則不檢測碰撞
    if (character->IsInvincible()) {
        return false;
    }

    if (CheckCollisionInternal(character)) {
        character->TakeDamage(1);
        return true;
    }

    return false;
}
