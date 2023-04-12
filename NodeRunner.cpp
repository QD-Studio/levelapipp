#include "DatabaseControllerThreads.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>

#include "GDServer.h"
#include "Tools.h"
#include "GDServer_BoomlingsLike21.h"
#include "GDServer_BoomlingsLike19.h"
#include "lapi_database.h"
#include "message.h"

using namespace LevelAPI;
using namespace std::chrono_literals;

void DatabaseController::node_runner_recentBot(Node *nd) {
    if(!nd->m_pPolicy->m_bEnableRecentTab) return;
start:
    std::this_thread::sleep_for(std::chrono::milliseconds((int)(nd->m_pPolicy->m_nQueueProcessingInterval * 1000)));
    if(nd->m_uQueue->m_vCommandQueue->size() == 0) {
        nd->m_uQueue->m_vCommandQueue->push_back(new NodeCommandQueue(NC_RECENT, new std::string("-")));
        nd->m_uQueue->save();
    }
    goto start;
}

void DatabaseController::node_runner_waitResolverRL(Node *nd, int rate_limit_length) {
    if(nd->m_bRateLimitApplied) return;
    
    nd->m_bRateLimitApplied = true;
    
    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI resolver " << *nd->m_sInternalName << "] Waiting " << rate_limit_length << "s" << std::endl; 
    std::this_thread::sleep_for(std::chrono::seconds(rate_limit_length + (int)nd->m_pPolicy->m_nResolverInterval));
    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI resolver " << *nd->m_sInternalName << "] Rate limit end!" << std::endl; 

    nd->m_bRateLimitApplied = false;
    return;
}

void DatabaseController::node_runner(Node *nd) {
    std::cout << "[LevelAPI] Running node " << nd->m_sInternalName->c_str() << std::endl;

    std::vector<int> recent_downloadedids;

    Backend::GDServer *server;
    switch(nd->m_uDatabase->m_nFeatureSet) {
        case 21: {
            server = static_cast<Backend::GDServer *>(new Backend::GDServer_BoomlingsLike21(nd->m_uDatabase->m_sEndpoint));
            break;
        }
        case 19: {
            server = static_cast<Backend::GDServer *>(new Backend::GDServer_BoomlingsLike19(nd->m_uDatabase->m_sEndpoint));
            break;
        }
    }

    // auto server = new Backend::GDServer_BoomlingsLike21(nd->m_uDatabase->m_sEndpoint);
    server->setDebug(false);

    std::thread rcbt(DatabaseController::node_runner_recentBot, nd);
    rcbt.detach();

    int waittime = 0;
    Level *llevel;

    goto start_linear;

error_ignore:
    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI downloader " << *nd->m_sInternalName << "] No stuff in the queue. Skipping" << std::endl;
    goto run_again;
loop_readonly2:
    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI " << *nd->m_sInternalName << "] Running node in READ-ONLY mode!" << std::endl;
    //delete server;
loop_readonly:
    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI " << *nd->m_sInternalName << "] Running node in READ-ONLY mode!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2s));
    goto loop_readonly;
run_again:
    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI " << *nd->m_sInternalName << "] run again" << std::endl;
    if(nd->m_bRateLimitApplied) {
        std::thread rlt(DatabaseController::node_runner_waitResolverRL, nd, waittime);
        rlt.detach();
    }
start_linear:
    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI " << *nd->m_sInternalName << "] start_linear" << std::endl;
    if (nd->m_uDatabase->m_bReadOnly) goto loop_readonly2;

    if(!nd->m_pPolicy->m_bEnableLinearResolver) goto start;
    nd->m_uQueue->m_bExecuteQueue = !nd->m_uQueue->m_bExecuteQueue;
    nd->m_uQueue->save();
    if(nd->m_uQueue->m_bExecuteQueue || nd->m_bRateLimitApplied) goto start;
    else {
        if(nd->m_pPolicy->m_bEnableResolver) {
            if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI linear resolver " << *nd->m_sInternalName << "] Waiting " << nd->m_pPolicy->m_nResolverInterval<< "s" << std::endl;
        
            std::this_thread::sleep_for(
                std::chrono::milliseconds(
                    (int)(nd->m_pPolicy->m_nResolverInterval * 1000.f)
                )
            );

            if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI linear resolver " << *nd->m_sInternalName << "] Fetching level " << nd->m_uQueue->m_nRuntimeState << "..." << std::endl;

            llevel = server->getLevelMetaByID(nd->m_uQueue->m_nRuntimeState, false);

            if (llevel->m_nRetryAfter == 0) {
                nd->initLevel(llevel);
                llevel->m_bHasLevelString = true;
                llevel->save();
                nd->m_jLastDownloadedLevel = llevel->levelJson;
                if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI linear resolver " << *nd->m_sInternalName << "] Fetched level " << nd->m_uQueue->m_nRuntimeState << std::endl;
            } else if (llevel->m_nRetryAfter > 0 && nd->m_pPolicy->m_bWaitResolverRL) {
                waittime = llevel->m_nRetryAfter;
                if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI linear resolver " << *nd->m_sInternalName << "] RATE LIMIT for " << waittime << "s" << std::endl;
                nd->m_bRateLimitApplied = true;
            }

            nd->m_uQueue->m_nRuntimeState++;   
        } else {
            if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI " << *nd->m_sInternalName << "] WARN: tried to run linear resolver with disabled resolver! " << std::endl;
        }
        goto run_again;
    }
start:
    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI " << *nd->m_sInternalName << "] start" << std::endl;
    if(!nd->m_pPolicy->m_bEnableLinearResolver) {
        if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI downloader " << *nd->m_sInternalName << "] Waiting " << nd->m_pPolicy->m_nQueueProcessingInterval << "s" << std::endl;

        std::this_thread::sleep_for(
            std::chrono::milliseconds((int)(nd->m_pPolicy->m_nQueueProcessingInterval * 1000.f))
        );
    }

    if(nd->m_uQueue->m_vCommandQueue->size() == 0) goto error_ignore;

    auto q = nd->m_uQueue->m_vCommandQueue->at(0);

    nd->m_uQueue->currentState = q->m_nCommand;

    switch(q->m_nCommand) {
        case NC_RECENT: {
            if(!nd->m_pPolicy->m_bEnableRecentTab) break;

            if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI downloader " << *nd->m_sInternalName << "] Fetching recent levels..." << std::endl;
            auto levels = server->getLevelsBySearchType(4);

            int i = 0;

            std::vector<int> new_levels;

            if((nd->m_bRateLimitApplied && nd->m_pPolicy->m_bWaitResolverRL) || !nd->m_pPolicy->m_bEnableResolver) {
                while(i < levels.size()) {
                    int levelid;
                    std::string levelname;
                    levelid = levels[i]->m_nLevelID;
                    levelname = std::string(levels[i]->m_sLevelName->c_str());
                    
                    nd->initLevel(levels[i]);
                    levels[i]->m_bHasLevelString = false;
                    levels[i]->save();
                    nd->m_jLastDownloadedLevel = levels[i]->levelJson;
                    levels[i]->m_sLinkedNode = std::string(nd->m_sInternalName->c_str());

                    if(!std::count(recent_downloadedids.begin(), recent_downloadedids.end(), levels[i]->m_nLevelID)) {
                        new_levels.push_back(levelid);
                        recent_downloadedids.push_back(levelid);
                        if (!DatabaseController::database->m_sRegisteredCID.empty() && DatabaseController::database->m_bBotReady) {
                            DatabaseController::database->m_pLinkedBot->m_pBot->message_create(dpp::message(
                                dpp::snowflake(DatabaseController::database->m_sRegisteredCID), levels[i]->getAsEmbed()
                            ));
                        }
                    }

                    delete levels[i];
                    levels[i] = nullptr;

                    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI downloader " << *nd->m_sInternalName << "] Level saved without level string (RL state): " << levelid << " \"" << levelname << "\"" << std::endl; 

                    i++;
                }
            } else {
                while(i < levels.size()) {
                    if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI resolver " << *nd->m_sInternalName << "] Waiting " << (float)waittime + nd->m_pPolicy->m_nResolverInterval << "s" << std::endl; 
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds((int)(waittime * 1000.f) + (int)(nd->m_pPolicy->m_nResolverInterval * 1000.f))
                    );

                    // if(!nd->m_pPolicy->m_bNoOutput) std::cout << "Level " << levels[i]->m_nLevelID << std::endl;

                    auto level = server->resolveLevelData(levels[i]);
                    waittime = level->m_nRetryAfter;
                    int levelid;
                    std::string levelname;
                    if(waittime == 0) {
                        levelid = level->m_nLevelID;
                        levelname = std::string(level->m_sLevelName->c_str());
                        nd->initLevel(level);
                        level->m_bHasLevelString = true;
                        level->save();
                        level->m_sLinkedNode = std::string(nd->m_sInternalName->c_str());
                        nd->m_jLastDownloadedLevel = level->levelJson;
                        if(!std::count(recent_downloadedids.begin(), recent_downloadedids.end(), levels[i]->m_nLevelID)) {
                            new_levels.push_back(levelid);
                            recent_downloadedids.push_back(levelid);
                            if (!DatabaseController::database->m_sRegisteredCID.empty() && DatabaseController::database->m_bBotReady) {
                                DatabaseController::database->m_pLinkedBot->m_pBot->message_create(dpp::message(
                                    dpp::snowflake(DatabaseController::database->m_sRegisteredCID), levels[i]->getAsEmbed()
                                ));
                                if(levelid == 90000000) {
                                    DatabaseController::database->m_pLinkedBot->m_pBot->message_create(dpp::message(
                                        dpp::snowflake(DatabaseController::database->m_sRegisteredCID), "@everyone WE DID IT LETS GO"
                                    ));
                                }
                            }
                        }
                        delete level;
                        level = nullptr;
                        if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI resolver " << *nd->m_sInternalName << "] Resolved level " << levelid << " \"" << levelname << "\"" << std::endl; 
                    } else {
                        if(!nd->m_pPolicy->m_bWaitResolverRL || level->m_nRetryAfter < 0) break;
                        if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI resolver " << *nd->m_sInternalName << "] RATE LIMIT for " << waittime << "s" << std::endl;
                        nd->m_bRateLimitApplied = true;
                        delete level;
                        level = nullptr;
                        break;
                    }

                    i++;
                }
            }
            levels.clear();

            break;
        }
        case NC_ID: {
            int id = std::stoi(std::string(q->m_sText->c_str()));
            float time_to_wait = nd->m_pPolicy->m_nResolverInterval - nd->m_pPolicy->m_nQueueProcessingInterval;
            if(time_to_wait < 0.f) time_to_wait = 0;
            if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI downloader " << *nd->m_sInternalName << "] Sleeping " << time_to_wait << "s and then fetching level " << id << std::endl;
            
            std::this_thread::sleep_for(
                std::chrono::milliseconds(
                    (int)(time_to_wait * 1000.f)
                )
            );

            if(nd->m_bRateLimitApplied && nd->m_pPolicy->m_bWaitResolverRL) {
                if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI resolver " << *nd->m_sInternalName << "] Impossible to fetch: Rate Limit" << id << std::endl;
                break;
            }
            if(!nd->m_pPolicy->m_bEnableResolver) {
                if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI resolver " << *nd->m_sInternalName << "] Impossible to fetch: Disabled" << id << std::endl;
                break;
            }

            auto level = server->getLevelMetaByID(id, false);
            if(level->m_nRetryAfter != 0) {
                if(!nd->m_pPolicy->m_bWaitResolverRL || level->m_nRetryAfter < 0) break;
                waittime = level->m_nRetryAfter;
                if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI resolver " << *nd->m_sInternalName << "] RATE LIMIT for " << waittime << "s" << std::endl;
                nd->m_bRateLimitApplied = true;
                delete level;
                level = nullptr;
            } else {
                nd->initLevel(level);
                level->m_bHasLevelString = true;
                level->save();
                nd->m_jLastDownloadedLevel = level->levelJson;
                if(!nd->m_pPolicy->m_bNoOutput) std::cout << "[LevelAPI downloader " << *nd->m_sInternalName << "] Fetched level " << id << std::endl;
                delete level;
                level = nullptr;
            }

            break;
        }
        default:
        case NC_NONE:
        case NC_IDLE: break;
    }
    delete q;
    q = nullptr;
    nd->m_uQueue->m_vCommandQueue->erase(nd->m_uQueue->m_vCommandQueue->begin());
    nd->m_uQueue->save();

    nd->m_uQueue->currentState = NC_NONE;

    goto run_again;
}
