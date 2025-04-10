// src/Attack/AttackPatternFactory.cpp
#include "Attack/AttackPatternFactory.hpp"
#include "Util/Logger.hpp"
#include <cmath>

AttackPatternFactory& AttackPatternFactory::GetInstance() {
    static AttackPatternFactory instance;
    return instance;
}

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateSingleCirclePattern(
    const glm::vec2& position,
    float radius,
    float delay) {

    auto pattern = std::make_shared<AttackPattern>();

    // 創建單個圓形攻擊
    auto attack = std::make_shared<CircleAttack>(position, delay, radius);

    // 將攻擊添加到模式中
    pattern->AddAttack(attack, 0.0f);

    // 設置模式總持續時間
    pattern->SetDuration(delay + 1.0f);

    return pattern;
}

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateMultiCirclePattern(
    const std::vector<glm::vec2>& positions,
    float radius,
    float delay,
    float interval) {

    auto pattern = std::make_shared<AttackPattern>();

    // 創建多個圓形攻擊
    float startTime = 0.0f;
    int sequenceNumber = 1;

    for (const auto& position : positions) {
        auto attack = std::make_shared<CircleAttack>(position, delay, radius, sequenceNumber++);
        pattern->AddAttack(attack, startTime);
        startTime += interval;
    }
    pattern->SetDuration(startTime + delay + 1.0f);

    return pattern;
}

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateRectanglePattern(
    const glm::vec2& position,
    float width,
    float height,
    float rotation,
    float delay) {

    auto pattern = std::make_shared<AttackPattern>();

    // 創建矩形攻擊
    auto attack = std::make_shared<RectangleAttack>(position, delay, width, height, rotation);

    // 將攻擊添加到模式中
    pattern->AddAttack(attack, 0.0f);

    // 設置模式總持續時間
    pattern->SetDuration(delay + 1.0f);

    return pattern;
}

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateLaserPattern(
    const glm::vec2& position,
    RectangleAttack::Direction direction,
    float width,
    float length,
    float delay) {

    auto pattern = std::make_shared<AttackPattern>();

    // 創建雷射攻擊（使用整合後的RectangleAttack）
    auto attack = std::make_shared<RectangleAttack>(position, delay, direction, width, length);

    // 將攻擊添加到模式中
    pattern->AddAttack(attack, 0.0f);

    // 設置模式總持續時間
    pattern->SetDuration(delay + 1.0f);

    return pattern;
}

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateMultiLaserPattern(
    const std::vector<glm::vec2>& positions,
    const std::vector<RectangleAttack::Direction>& directions,
    float width,
    float length,
    float delay,
    float interval) {

    auto pattern = std::make_shared<AttackPattern>();

    // 創建多個雷射攻擊
    float startTime = 0.0f;
    int sequenceNumber = 1;

    // 確保方向和位置數組長度相同，如果方向數組較短，循環使用
    size_t directionCount = directions.size();

    for (size_t i = 0; i < positions.size(); ++i) {
        const auto& position = positions[i];
        const auto& direction = directions[i % directionCount]; // 循環使用方向

        // 使用整合後的RectangleAttack創建雷射
        auto attack = std::make_shared<RectangleAttack>(
            position, delay, direction, width, length, sequenceNumber++
        );

        pattern->AddAttack(attack, startTime);
        startTime += interval;
    }

    // 設置模式總持續時間
    pattern->SetDuration(startTime + delay + 1.0f);

    return pattern;
}

std::vector<glm::vec2> AttackPatternFactory::CalculateCircularPositions(
    const glm::vec2& center,
    float radius,
    int count,
    float startAngle) {

    std::vector<glm::vec2> positions;
    positions.reserve(count);

    // 計算環上的每個點
    float angleStep = 2.0f * M_PI / static_cast<float>(count);

    for (int i = 0; i < count; ++i) {
        float angle = startAngle + i * angleStep;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        positions.emplace_back(x, y);
    }

    return positions;
}

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateCircularPattern(
    const glm::vec2& centerPosition,
    float radius,
    float attackRadius,
    int count,
    float delay,
    float interval) {

    // 計算環上的攻擊位置
    std::vector<glm::vec2> positions = CalculateCircularPositions(centerPosition, radius, count);

    // 創建多圓攻擊模式
    auto pattern = CreateMultiCirclePattern(positions, attackRadius, delay, interval);

    // 添加移動敵人到中心位置的命令
    pattern->AddEnemyMovement([centerPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(centerPosition);
        LOG_DEBUG("Enemy moved to center position: ({}, {})", centerPosition.x, centerPosition.y);
    }, 0.0f);

    return pattern;
}

// 在 AttackPatternFactory.cpp 中修正 CreateCrossRotatingLaserPattern 方法實現
std::shared_ptr<AttackPattern> AttackPatternFactory::CreateCrossRotatingLaserPattern(
    const glm::vec2& centerPosition,
    float width,
    float height,
    float rotationSpeed,
    float duration,
    float delay) {

    auto pattern = std::make_shared<AttackPattern>();

    // 創建兩個矩形攻擊（一個水平，一個垂直）組成十字形
    auto horizontalAttack = std::make_shared<RectangleAttack>(
        centerPosition, delay, width, height, 0.0f, 1
    );

    auto verticalAttack = std::make_shared<RectangleAttack>(
        centerPosition, delay, width, height, 1.57f, 2  // 1.57 rad ≈ 90度
    );

    // 設置自動旋轉和攻擊持續時間
    horizontalAttack->SetAutoRotation(true, rotationSpeed);
    horizontalAttack->SetAttackDuration(duration);

    verticalAttack->SetAutoRotation(true, rotationSpeed);
    verticalAttack->SetAttackDuration(duration);

    LOG_DEBUG("Created cross rotating laser attacks with duration: {}, rotationSpeed: {}",
              duration, rotationSpeed);

    // 將攻擊添加到模式中
    pattern->AddAttack(horizontalAttack, 0.0f);
    pattern->AddAttack(verticalAttack, 0.0f);

    // 設置模式總持續時間
    pattern->SetDuration(delay + duration + 1.0f);

    return pattern;
}

// 修改 AttackPatternFactory.cpp 中的 CreateMovingCirclePattern 方法

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateMovingCirclePattern(
    const glm::vec2& startPosition,
    const glm::vec2& endPosition,
    float radius,
    float speed,
    float delay) {

    auto pattern = std::make_shared<AttackPattern>();

    // 創建圓形攻擊
    auto circleAttack = std::make_shared<CircleAttack>(startPosition, delay, radius, 1);

    // 計算移動方向和距離
    glm::vec2 direction = endPosition - startPosition;
    float distance = glm::length(direction);
    direction = glm::normalize(direction);

    // 計算持續時間（基於距離和速度）
    float moveDuration = distance / speed;

    // 設置移動特效參數
    circleAttack->SetColor(Util::Color::FromRGB(255, 100, 0, 200));

    // 保存攻擊方向和速度供特效創建時使用
    circleAttack->SetMovementParams(direction, speed, distance);

    // 明確設置攻擊持續時間，確保足夠長
    circleAttack->SetAttackDuration(moveDuration + 0.5f);  // 加一點緩衝時間

    // 加入攻擊模式
    pattern->AddAttack(circleAttack, 0.0f);

    // 設置模式總持續時間
    pattern->SetDuration(delay + moveDuration + 1.0f);

    LOG_DEBUG("Created moving circle attack from ({}, {}) to ({}, {}), speed: {}, duration: {}",
              startPosition.x, startPosition.y, endPosition.x, endPosition.y,
              speed, moveDuration);

    return pattern;
}

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateCornerBulletPattern(
    int bulletCount,
    float bulletSpeed,
    float bulletRadius,
    float delay) {

    auto pattern = std::make_shared<AttackPattern>();

    // 創建角落子彈攻擊
    auto attack = std::make_shared<CornerBulletAttack>(delay, bulletCount);
    attack->SetBulletSpeed(bulletSpeed);
    attack->SetBulletRadius(bulletRadius);

    // 將攻擊添加到模式中
    pattern->AddAttack(attack, 0.0f);

    // 設置模式總持續時間
    pattern->SetDuration(delay + 3.0f);  // 給予足夠時間讓子彈飛行

    return pattern;
}


// BATTLE 1 特殊攻擊模式 - 簡單的圓形和矩形攻擊組合
std::shared_ptr<AttackPattern> AttackPatternFactory::CreateBattle1Pattern() {
    auto pattern = std::make_shared<AttackPattern>();

    // 開場設置 - 敵人移動到場地中央
    glm::vec2 centerPosition(0.0f, 50.0f);
    pattern->AddEnemyMovement([centerPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(centerPosition);
        LOG_DEBUG("Enemy moved to center for Battle 1");
    }, 0.0f);

    // 第一波攻擊：單個大圓形攻擊
    auto attack1 = std::make_shared<CircleAttack>(centerPosition, 3.0f, 200.0f, 1);
    attack1->SetColor(Util::Color::FromRGB(255, 50, 50, 180));
    pattern->AddAttack(attack1, 1.0f);

    // 第二波攻擊：三個水平排列的小圓形攻擊
    std::vector<glm::vec2> positions = {
        {centerPosition.x - 250.0f, centerPosition.y},
        {centerPosition.x, centerPosition.y},
        {centerPosition.x + 250.0f, centerPosition.y}
    };

    float startTime = 5.0f;
    for (int i = 0; i < positions.size(); ++i) {
        auto attack = std::make_shared<CircleAttack>(positions[i], 2.0f, 120.0f, i + 2);
        attack->SetColor(Util::Color::FromRGB(0, 150, 255, 150));
        pattern->AddAttack(attack, startTime);
        startTime += 0.5f;
    }

    // 第三波攻擊：中央大矩形攻擊
    auto attack2 = std::make_shared<RectangleAttack>(
        centerPosition, 2.5f, 2000.0f, 160.0f, 1.57f, 5
    );
    attack2->SetColor(Util::Color::FromRGB(255, 150, 0, 180));
    pattern->AddAttack(attack2, 8.5f);

    // 敵人移動到場地右側
    glm::vec2 rightPosition(200.0f, 50.0f);
    pattern->AddEnemyMovement([rightPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(rightPosition);
        LOG_DEBUG("Enemy moved to right side");
    }, 11.0f);

    // 設置模式總持續時間
    pattern->SetDuration(15.0f);

    return pattern;
}

// BATTLE 2 特殊攻擊模式 - 雷射和環形攻擊組合
std::shared_ptr<AttackPattern> AttackPatternFactory::CreateBattle2Pattern() {
    auto pattern = std::make_shared<AttackPattern>();

    // 開場設置 - 敵人移動到場地中央偏上位置
    glm::vec2 topPosition(0.0f, 150.0f);
    pattern->AddEnemyMovement([topPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(topPosition);
        LOG_DEBUG("Enemy moved to top for Battle 2");
    }, 0.0f);

    // 第一波攻擊：從上到下的雷射掃射
    std::vector<glm::vec2> laserPositions = {
        {-400.0f, topPosition.y},
        {-200.0f, topPosition.y},
        {0.0f, topPosition.y},
        {200.0f, topPosition.y},
        {400.0f, topPosition.y}
    };


    float startTime = 1.0f;
    for (int i = 0; i < laserPositions.size(); ++i) {
        auto attack = std::make_shared<RectangleAttack>(
            laserPositions[i], 1.0f, RectangleAttack::Direction::VERTICAL, 1000.0f, 60.0f, i + 1
        );
        pattern->AddAttack(attack, startTime);
        startTime += 0.5f;
    }

    // 敵人移動到場地中央
    glm::vec2 centerPosition(0.0f, 0.0f);
    pattern->AddEnemyMovement([centerPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(centerPosition);
        LOG_DEBUG("Enemy moved to center");
    }, 6.0f);

    // 第二波攻擊：環形圓形攻擊
    std::vector<glm::vec2> circlePositions = CalculateCircularPositions(centerPosition, 250.0f, 8);

    startTime = 8.0f;
    for (int i = 0; i < circlePositions.size(); ++i) {
        auto attack = std::make_shared<CircleAttack>(
            circlePositions[i], 2.0f, 200.0f, i + 6
        );
        attack->SetColor(Util::Color::FromRGB(200, 0, 200, 180));
        pattern->AddAttack(attack, startTime);
        startTime += 0.3f;
    }

    // 第三波攻擊：十字形雷射組合
    auto attackH = std::make_shared<RectangleAttack>(
        centerPosition, 3.0f, RectangleAttack::Direction::HORIZONTAL, 2000.0f, 100.0f, 14
    );
    pattern->AddAttack(attackH, 12.0f);
    auto attackV = std::make_shared<RectangleAttack>(
        centerPosition, 3.0f, RectangleAttack::Direction::VERTICAL, 2000.0f, 100.0f, 15
    );
    pattern->AddAttack(attackV, 12.0f);
    auto attack1 = std::make_shared<RectangleAttack>(
        centerPosition, 3.0f, RectangleAttack::Direction::DIAGONAL_TL_BR, 2000.0f, 100.0f, 16
    );
    pattern->AddAttack(attack1, 12.0f);
    auto attack4 = std::make_shared<RectangleAttack>(
        centerPosition, 3.0f, RectangleAttack::Direction::DIAGONAL_TR_BL, 2000.0f, 100.0f, 17
    );
    pattern->AddAttack(attack4, 12.0f);

    // 敵人移動回原位
    glm::vec2 finalPosition(200.0f, 0.0f);
    pattern->AddEnemyMovement([finalPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(finalPosition);
        LOG_DEBUG("Enemy moved to final position");
    }, 16.0f);

    // 設置模式總持續時間
    pattern->SetDuration(20.0f);

    return pattern;
}