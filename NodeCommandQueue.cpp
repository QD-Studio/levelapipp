#include "lapi_database.h"
#include "json/single_include/nlohmann/json.hpp"

using namespace LevelAPI::DatabaseController;

NodeCommandQueue::NodeCommandQueue(int command, std::string text) {
    m_nCommand = command;
    m_sText = text;

    setupJSON();

    commandJson["type"] = command;
    commandJson["data"] = text;
}

NodeCommandQueue::NodeCommandQueue() {
    NodeCommandQueue(0, "-");
}

void NodeCommandQueue::save() {
    commandJson["type"] = m_nCommand;
    commandJson["data"] = m_sText;
}

void NodeCommandQueue::recover() {
    m_nCommand = commandJson["type"].get<int>();
    m_sText = commandJson["data"].get<std::string>();
}

void NodeCommandQueue::setupJSON() {
    commandJson = nlohmann::json();
}