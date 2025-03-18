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

    // 設置模式總持續時間
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

std::shared_ptr<AttackPattern> AttackPatternFactory::CreateMultiLaserPattern(
    const std::vector<glm::vec2>& positions,
    const std::vector<LaserAttack::Direction>& directions,
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

        auto attack = std::make_shared<LaserAttack>(position, delay, direction, width, length, sequenceNumber++);
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
        centerPosition, 2.5f, 400.0f, 120.0f, 0.0f, 5
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
        auto attack = std::make_shared<LaserAttack>(
            laserPositions[i], 1.5f, LaserAttack::Direction::VERTICAL, 60.0f, 1000.0f, i + 1
        );
        pattern->AddAttack(attack, startTime);
        startTime += 0.7f;
    }

    // 敵人移動到場地中央
    glm::vec2 centerPosition(0.0f, 0.0f);
    pattern->AddEnemyMovement([centerPosition](std::shared_ptr<Enemy> enemy) {
        enemy->SetPosition(centerPosition);
        LOG_DEBUG("Enemy moved to center");
    }, 7.0f);

    // 第二波攻擊：環形圓形攻擊
    std::vector<glm::vec2> circlePositions = CalculateCircularPositions(centerPosition, 250.0f, 8);

    startTime = 8.0f;
    for (int i = 0; i < circlePositions.size(); ++i) {
        auto attack = std::make_shared<CircleAttack>(
            circlePositions[i], 2.0f, 100.0f, i + 6
        );
        attack->SetColor(Util::Color::FromRGB(200, 0, 200, 180));
        pattern->AddAttack(attack, startTime);
        startTime += 0.3f;
    }

    // 第三波攻擊：十字形雷射組合
    auto attackH = std::make_shared<LaserAttack>(
        centerPosition, 3.0f, LaserAttack::Direction::HORIZONTAL, 80.0f, 2000.0f, 14
    );
    pattern->AddAttack(attackH, 12.0f);

    auto attackV = std::make_shared<LaserAttack>(
        centerPosition, 3.0f, LaserAttack::Direction::VERTICAL, 80.0f, 2000.0f, 15
    );
    pattern->AddAttack(attackV, 12.0f);

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