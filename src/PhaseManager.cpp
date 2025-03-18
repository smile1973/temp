#include "PhaseManger.hpp"

#include "Util/Logger.hpp"

PhaseManager::PhaseManager() {
    m_Background = std::make_shared<BackgroundImage>();
}

void PhaseManager::NextPhase() {
    LOG_DEBUG("Passed! Next phase: {}", m_Phase);
    m_Background->NextPhase(m_Phase++);
}
