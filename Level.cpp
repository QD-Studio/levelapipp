#include "Level.h"
#include "lapi_database.h"
#include "json/single_include/nlohmann/json.hpp"
#include "gmd2pp/gmd2.h"
#include "message.h"
#include <fstream>
#include "Time.h"
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/core/hal/interface.h>
#include <opencv4/opencv2/core/mat.hpp>
#include <random>
#include "Translation.h"
#include <string>
#include <sys/stat.h>
#include <vector>

#include <opencv2/imgproc.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

using namespace LevelAPI::DatabaseController;
using namespace LevelAPI::Frontend;

Level::Level() {
    m_uRelease = new LevelRelease();

    m_nRetryAfter = 0;
    m_bHasLevelString = false;
    m_nAccountID = 0;
    m_uRelease->m_nGameVersion = 0;

    m_nLevelID = 0;
    m_nVersion = 0;
    m_nPlayerID = 0;
    m_nDownloads = 0;
    m_nMusicID = 0;
    m_nLikes = 0;
    m_nLength = 0;
    m_nDifficultyDenominator = 0;
    m_nDifficultyNumerator = 0;
    m_nSetCompletes = 0;
    m_nGameVersion = 0;
    m_nDislikes = 0;
    m_nStars = 0;
    m_nFeatureScore = 0;
    m_nCoins = 0;
    m_nCopiedID = 0;
    m_nStarsRequested = 0;
    m_nDailyNumber = 0;
    m_nEpic = 0;
    m_nDemonDifficulty = 0;
    m_nObjects = 0;
    m_nEditorTime = 0;
    m_nEditorTimeTotal = 0;

    m_bAuto = false;
    m_bDemon = false;
    m_bVerifiedCoins = false;
    m_bLDM = false;
    m_bGauntlet = false;
    m_b2P = false;

    Frontend::Time *t = Frontend::Time::create();
    m_sCreatedTimestamp = t->getAsString();
    delete t;
    t = nullptr;

    setupJSON();
}

void Level::setupJSON() {
    levelJson = nlohmann::json();
}

void Level::save(bool onlyLevelString) {
    #define fill(str, val) levelJson[str] = val;
    #define file_exists(cstr) (stat(cstr, &buffer) == 0)

    fill("levelID", m_nLevelID)
    fill("version", m_nVersion)
    fill("playerID", m_nPlayerID)
    fill("accountID", m_nAccountID)
    fill("downloads", m_nDownloads)
    fill("musicID", m_nMusicID)
    fill("likes", m_nLikes)
    fill("length", m_nLength)
    fill("difficulty_denominator", m_nDifficultyDenominator)
    fill("difficulty_numerator", m_nDifficultyNumerator)
    fill("fakeGameVersion", m_nGameVersion)
    fill("dislikes", m_nDislikes)
    fill("stars", m_nStars)
    fill("featureScore", m_nFeatureScore)
    fill("copiedFrom", m_nCopiedID)
    fill("dailyNumber", m_nDailyNumber)
    fill("coins", m_nCoins)
    fill("starsRequested", m_nStarsRequested)
    fill("isEpic", (bool)m_nEpic)
    fill("demonDifficulty", m_nDemonDifficulty)
    fill("editorTime", m_nEditorTime)
    fill("editorTimeTotal", m_nEditorTimeTotal)
    fill("songID", m_nSongID)
    fill("objects", m_nObjects);

    fill("isAuto", m_bAuto)
    fill("isDemon", m_bDemon)
    fill("areCoinsVerified", m_bVerifiedCoins)
    fill("ldmAvailable", m_bLDM)
    fill("is2P", m_b2P)

    fill("levelName", m_sLevelName)
    fill("levelDescription", m_sDescription)
    fill("uploadDate", m_sUploadDate)
    fill("updateDate", m_sUpdateDate)
    fill("username", m_sUsername);
    fill("actualGameVersion", m_uRelease->m_fActualVersion)
    fill("publicationDate", m_sCreatedTimestamp);

    std::string g = m_sLevelPath + "/meta.json";
    std::string g2 = m_sLevelPath + "/data.gmd2";
    
    if (!onlyLevelString) {
	    std::ofstream file(g);
    	file << levelJson.dump(4, ' ', false, nlohmann::json::error_handler_t::ignore);
	    file.close();
    }

    if(m_bHasLevelString) {
        struct stat buffer;
        if(!file_exists(g2.c_str())) {
            auto gmd2file = new GMD2();
            gmd2file->setFileName(g2);
            gmd2file->setDebug(false);
            gmd2file->setLevel(this);
            gmd2file->generate();

            delete gmd2file;
            gmd2file = nullptr;
        }
    }

    return;
}

void Level::restore() {
    #define RS(t, str, val) if (levelJson.contains(str)) {if(!levelJson[str].is_null()) val = levelJson[str].get<t>();}

    RS(int, "levelID", m_nLevelID)
    RS(int, "version", m_nVersion)
    RS(int, "playerID", m_nPlayerID)
    RS(int, "downloads", m_nDownloads)
    RS(int, "musicID", m_nMusicID)
    RS(int, "likes", m_nLikes)
    RS(int, "length", m_nLength)
    RS(int, "difficulty_denominator", m_nDifficultyDenominator)
    RS(int, "difficulty_numerator", m_nDifficultyNumerator)
    RS(int, "fakeGameVersion", m_nGameVersion)
    RS(int, "dislikes", m_nDislikes)
    RS(int, "stars", m_nStars)
    RS(int, "featureScore", m_nFeatureScore)
    RS(int, "copiedFrom", m_nCopiedID)
    RS(int, "dailyNumber", m_nDailyNumber)
    RS(int, "coins", m_nCoins)
    RS(int, "starsRequested", m_nStarsRequested)
    RS(int, "isEpic", m_nEpic)
    RS(int, "demonDifficulty", m_nDemonDifficulty)
    RS(int, "editorTime", m_nEditorTime)
    RS(int, "editorTimeTotal", m_nEditorTimeTotal)
    RS(int, "accountID", m_nAccountID)
    RS(int, "songID", m_nSongID)
    RS(int, "objects", m_nObjects)

    RS(bool, "isAuto", m_bAuto)
    RS(bool, "isDemon", m_bDemon)
    RS(bool, "areCoinsVerified", m_bVerifiedCoins)
    RS(bool, "ldmAvailable", m_bLDM)
    RS(bool, "is2P", m_b2P)

    RS(std::string, "levelName", m_sLevelName)
    RS(std::string, "levelDescription", m_sDescription)
    RS(std::string, "uploadDate", m_sUploadDate)
    RS(std::string, "updateDate", m_sUpdateDate)
    RS(std::string, "username", m_sUsername)
    RS(std::string, "actualGameVersion", m_uRelease->m_fActualVersion)
    RS(std::string, "publicationDate", m_sCreatedTimestamp);

    return;
}

Level::~Level() {
    delete m_uRelease;
    m_uRelease = nullptr;
}

void merge_images(cv::Mat* background, cv::Mat* upcoming, int x, int y)
{
    auto handle_cv_8uc4 = [=](int i, int j)
            {

                if(upcoming->at<cv::Vec4b>(j, i)[3] > 10)//10 is only epsilon for trash hold, you can put also 0 or anything else.
                {
                    background->at<cv::Vec4b>(y+j, x+i) = upcoming->at<cv::Vec4b>(j, i);
                }
            };

    auto handle_cv_8uc3 = [=](int i, int j)
    {
        background->at<cv::Vec4b>(y+j, x+i)[0] = upcoming->at<cv::Vec3b>(j, i)[0];
        background->at<cv::Vec4b>(y+j, x+i)[1] = upcoming->at<cv::Vec3b>(j, i)[1];
        background->at<cv::Vec4b>(y+j, x+i)[2] = upcoming->at<cv::Vec3b>(j, i)[2];
        background->at<cv::Vec4b>(y+j, x+i)[3] = 255;
    };

    for(int i = 0; i < upcoming->cols; i++)
    {
        for(int j = 0; j < upcoming->rows; j++)
        {
            if(j + y >= background->rows)
            {
                break;
            }

            if(x + i >= background->cols)
            {
                return;
            }

            switch(upcoming->channels())
            {
                case 3:
                {
                    handle_cv_8uc3(i, j);
                    break;
                }

                case 4:
                {
                    handle_cv_8uc4(i, j);
                    break;
                }

                default:
                {
                    //maybe error?
                }
            }

        }
    }
}

std::string Level::generateDifficultyImage(std::string folder_prefix) {
    std::string path;
    std::string file = "rating_";
    std::string diffimage;
    int stars;
    std::vector<bool> parameters;
    std::vector<cv::Mat> mats;

    parameters.push_back(m_nFeatureScore > 0);
    parameters.push_back(m_nEpic);
    if(parameters[1]) parameters[0] = false;

    if(parameters[0]) {
        mats.push_back(cv::imread(folder_prefix + "/feature.png", cv::IMREAD_UNCHANGED));  
    };
    if(parameters[1]) {
        mats.push_back(cv::imread(folder_prefix + "/epic.png", cv::IMREAD_UNCHANGED));
    };

    switch(m_nStars == 0 ? m_nStarsRequested : m_nStars) {
        case 1:
        case 2: {
            diffimage = "easy";
            break;
        }
        case 3: {
            diffimage = "normal";
            break;
        }
        case 4:
        case 5: {
            diffimage = "hard";
            break;
        }
        case 6:
        case 7: {
            diffimage = "harder";
            break;
        }
        case 8:
        case 9: {
            diffimage = "insane";
            break;
        }
        case 10: {
            diffimage = "demon";
            break;
        }
        default: {
            diffimage = "na";
            break;
        }
    }

    stars = m_nStars;
    if(stars <= 10 && stars >= 0) mats.push_back(cv::imread(folder_prefix + "/star" + std::to_string(stars) + ".png", cv::IMREAD_UNCHANGED));

    if(m_bAuto) diffimage = "auto";
    if(m_bDemon) diffimage = "demon";
    mats.push_back(cv::imread(folder_prefix + "/" + diffimage + ".png", cv::IMREAD_UNCHANGED));

    file += diffimage + std::to_string(stars) + "_";
    int i = 0;
    while(i < parameters.size()) {
        file += (parameters[i]) ? "y" : "n";
        i++;
    }
    file += "_new.png";

    path = folder_prefix + "/" + file;

    cv::Mat final = cv::imread(folder_prefix + "/empty.png", cv::IMREAD_UNCHANGED);
    cv::Mat final3 = final;
    
    i = 0;
    while(i < mats.size()) {
        merge_images(&final, &mats[i], 0, 0);
        i++;
    }

    //cv::Mat final2;

    //cv::resize(final, final2, cv::Size(), 1, 1, 2);

    cv::imwrite(path, final);

    return file;
}

dpp::embed Level::getAsEmbed(LevelAppearanceEvent e) {
    std::vector<std::string> eventtable = {
        "lapi.level.embed.description",
        "lapi.level.register.embed.description",
        "lapi.level.rate.embed.description",
    };

    std::string url = "https://levelapi.dogotrigger.xyz";

    std::string img_path = generateDifficultyImage("images");

    std::string gv = std::to_string((float)m_nGameVersion / 10.f);
    gv.erase(gv.find_last_not_of('0') + 1, std::string::npos);
    gv.erase(gv.find_last_not_of('.') + 1, std::string::npos);

    std::string msg = Translation::getByKey(eventtable[e], gv, m_sCreatedTimestamp);

    std::string thumbnail = url + "/api/v1/img/request/" + img_path;    

    std::random_device rd;
    std::uniform_int_distribution<int> uid(0, m_nLevelID);

    dpp::embed embed = dpp::embed().
        set_color(uid(rd)).
        set_title(Translation::getByKey("lapi.level.embed.title")).
        set_description(msg).
        add_field(
            Translation::getByKey("lapi.level.embed.field.id"),
            "**" + std::to_string(this->m_nLevelID) + "**",
            true
        ).
        add_field(
            Translation::getByKey("lapi.level.embed.field.name"),
            "**" + this->m_sLevelName + "**",
            true
        ).
        add_field(
            Translation::getByKey("lapi.level.embed.field.author"),
            "**" + this->m_sUsername + "**",
            true
        ).
        add_field(
            Translation::getByKey("lapi.level.embed.field.info"),
            "[" + Translation::getByKey("lapi.level.embed.field.info.value.metadata") + "](" + url + "/api/v1/level/download?id=" + std::to_string(this->m_nLevelID) + "&node=" + m_sLinkedNode + ")" + (m_bHasLevelString ? " | [GMD2](" + url + "/api/v1/level/download?id=" + std::to_string(this->m_nLevelID) + "&node=" + m_sLinkedNode + "&gmd2=true)" : ""),
            true
        ).
        set_thumbnail(thumbnail).
        set_footer(dpp::embed_footer().set_text("LevelAPI  •  " + m_sLinkedNode)).
        set_timestamp(time(0)
    );

    return embed;
}
