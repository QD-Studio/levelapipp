#include "lapi_database.h"
#include <vector>

using namespace LevelAPI::DatabaseController;

NodeQueue::NodeQueue(NodeCommandQueue *q, bool executeQueue, int runtimeState) {
    m_vCommandQueue = new std::vector<NodeCommandQueue *>();
    m_vCommandQueue->push_back(q);
    m_bExecuteQueue = executeQueue;
    m_nRuntimeState = runtimeState;
    
    setupJSON();

    int i = 0;
    while(i < m_vCommandQueue->size()) {
        queueJson["commandQueue"].push_back(m_vCommandQueue->at(i)->commandJson);
        i++;
    }
    queueJson["executeQueue"] = executeQueue;
    queueJson["runtimeState"] = runtimeState;
}
NodeQueue::NodeQueue(std::vector<NodeCommandQueue *> *vec, bool executeQueue, int runtimeState) {
    m_vCommandQueue = vec;
    m_bExecuteQueue = executeQueue;
    m_nRuntimeState = runtimeState;

    setupJSON();

    queueJson["commandQueue"] = nlohmann::json::array();

    int i = 0;
    while(i < m_vCommandQueue->size()) {
        queueJson["commandQueue"].push_back(m_vCommandQueue->at(i)->commandJson);
        i++;
    }
    queueJson["executeQueue"] = executeQueue;
    queueJson["runtimeState"] = runtimeState;
}
NodeQueue::NodeQueue(bool executeQueue, int runtimeState) {
    m_vCommandQueue = new std::vector<NodeCommandQueue *>();
    m_bExecuteQueue = executeQueue;
    m_nRuntimeState = runtimeState;

    setupJSON();

    queueJson["commandQueue"] = nlohmann::json::array();
    queueJson["executeQueue"] = executeQueue;
    queueJson["runtimeState"] = runtimeState;
}
NodeQueue::NodeQueue() {
    NodeQueue(false, 0);
}

void NodeQueue::save() {
    // queueJson["commandQueue"] = nlohmann::json::array();
    queueJson["executeQueue"] = m_bExecuteQueue;
    queueJson["runtimeState"] = m_nRuntimeState;

    int i = 0;
    while(i < m_vCommandQueue->size()) {
        m_vCommandQueue->at(i)->save();
        queueJson["commandQueue"] = m_vCommandQueue->at(i)->commandJson;
        i++;
    }
}

void NodeQueue::recover() {
    m_bExecuteQueue = queueJson["executeQueue"].get<bool>();
    m_nRuntimeState = queueJson["runtimeState"].get<int>();

    int i = 0;
    while(i < queueJson["commandQueue"].size()) {
        m_vCommandQueue->push_back(new NodeCommandQueue());
        m_vCommandQueue->at(i)->commandJson = queueJson["commandQueue"].at(i);
        m_vCommandQueue->at(i)->recover();
        i++;
    }
}

void NodeQueue::setupJSON() {
    queueJson = nlohmann::json();
}

NodeQueue::~NodeQueue() {
    printf("~nodequeue\n");
    int i = 0;
    while(i < m_vCommandQueue->size()) {
        delete m_vCommandQueue->at(i);
        i++;
    }
    delete m_vCommandQueue;
}