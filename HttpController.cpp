/**
 *  LevelAPI - Geometry Dash level cacher with search functionality and more.
    Copyright (C) 2023  Sergei Baigerov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "HttpController.h"

#include "json/single_include/nlohmann/json.hpp"

using namespace LevelAPI;

#include <string>
#include <fstream>
#include <sstream>
#include <thread>

nlohmann::json HttpController_json;

void HttpController::parse() {
    std::ifstream h("database/configuration/http.json");
    HttpController_json = nlohmann::json::parse(h);
    
}
int HttpController::getPort() {
    if(HttpController_json.contains("port")) {
        return HttpController_json["port"].get<int>();
    } else {
        return 8000;
    }
}
int HttpController::getThreads() {
    if (HttpController_json.contains("threads")) {
        return HttpController_json["threads"].get<int>();
    } else {
        return std::thread::hardware_concurrency();
    }
}
std::string HttpController::getURL() {
    if (HttpController_json.contains("url")) {
        return HttpController_json["url"].get<std::string>();
    } else {
        return "https://levelapi.dogotrigger.xyz";
    }
}

void HttpController::setup() {
    std::ifstream h("database/configuration/http.json");
    if(!h.good()) {
        nlohmann::json j;
        j["port"] = 8000;
        j["threads"] = std::thread::hardware_concurrency();
        j["url"] = "https://levelapi.dogotrigger.xyz";

        std::string j2 = j.dump();

        std::ofstream j3;
        j3.open ("database/configuration/http.json");
        j3 << j2;
        j3.close();
    }
}
void HttpController::save() {
    HttpController_json["port"] = getPort();
    HttpController_json["threads"] = getThreads();
    HttpController_json["url"] = getURL();

    std::string j2 = HttpController_json.dump();

    std::ofstream j3;
    j3.open ("database/configuration/http.json");
    j3 << j2;
    j3.close();
}

#include <fmt/format.h>

#include "GenericTools.hpp"
#include "Translation.h"
#include <raylib.h>

using namespace LevelAPI::Frontend;

std::string HttpController::generateIndex(std::vector<std::string> nodes, struct AutorunIndex autorun) {
    std::string url = getURL();
    std::string availableNodesArray = "[" + GenericTools::convertFromVector(nodes) + "]";
    std::string buttons;

    for (std::string node : nodes) {
        buttons += fmt::format("<button onclick=\"selectNode('{}')\" type=\"button\"><code><b>{}</b></code></button>\n", 
            node, node
        );
    }

    if (buttons.length() >= 1) {
        buttons.pop_back();
    }

    std::string autorun_code = "";

    if (autorun.enableAutorun) {
        if (!autorun.defaultNode.empty() && autorun.levelID == 0) {
            autorun_code = fmt::format("selectNode(\"{}\");", autorun.defaultNode);
        }
        if (!autorun.defaultNode.empty() && autorun.levelID != 0) {
            autorun_code = fmt::format("switchToLevel(\"{}\", {});", autorun.defaultNode, autorun.levelID);
        }
    }

    char *data = LoadFileText("resources/lapiui/index.html");

    if (!data) return "error: \"resources/lapiui/index.html\" cannot be found";

    size_t sz = strlen(data) * 2 + 1;
    char *fmtdata = (char *)malloc(sz);

    memset(fmtdata, 0, sz);

    snprintf(fmtdata, sz, data,
        availableNodesArray.c_str(),
        url.c_str(),
        Translation::getByKey("lapi.web.jobs").c_str(),
        Translation::getByKey("lapi.web.download-jobs").c_str(),
        Translation::getByKey("lapi.web.levels-downloaded").c_str(),
        Translation::getByKey("lapi.web.latest-level").c_str(),
        Translation::getByKey("lapi.web.by").c_str(),
        Translation::getByKey("lapi.web.length.short").c_str(),
        Translation::getByKey("lapi.web.length.medium").c_str(),
        Translation::getByKey("lapi.web.length.long").c_str(),
        Translation::getByKey("lapi.web.length.xl").c_str(),
        Translation::getByKey("lapi.web.length.plat").c_str(),
	Translation::getByKey("lapi.web.length.plat").c_str(),
        Translation::getByKey("lapi.web.by").c_str(),
        Translation::getByKey("lapi.web.invalid-node").c_str(),
        Translation::getByKey("lapi.web.unknown-level").c_str(),
        Translation::getByKey("lapi.web.invalid-node-p").c_str(),
        Translation::getByKey("lapi.web.invalid-node").c_str(),
        Translation::getByKey("lapi.web.no-levels").c_str(),
        Translation::getByKey("lapi.web.title").c_str(),
        Translation::getByKey("lapi.web.table.node").c_str(),
        Translation::getByKey("lapi.web.table.information").c_str(),
        Translation::getByKey("lapi.web.select-node").c_str(),
        buttons.c_str(),
        Translation::getByKey("lapi.web.level-app").c_str(),
        Translation::getByKey("lapi.web.search-levels").c_str(),
        Translation::getByKey("lapi.web.back").c_str(),
        Translation::getByKey("lapi.web.search").c_str(),
        Translation::getByKey("lapi.web.search-levels").c_str(),
        Translation::getByKey("lapi.web.search.id").c_str(),
        Translation::getByKey("lapi.web.search.stars").c_str(),
        Translation::getByKey("lapi.web.search.page").c_str(),
        Translation::getByKey("lapi.web.search.name").c_str(),
        Translation::getByKey("lapi.web.search.desc").c_str(),
        Translation::getByKey("lapi.web.search.nick").c_str(),
        Translation::getByKey("lapi.web.more-info").c_str(),
        Translation::getByKey("lapi.web.download-level").c_str(),
        Translation::getByKey("lapi.web.back").c_str(),
        autorun_code.c_str()
    );

    std::string ret = std::string(fmtdata);

    free(fmtdata);
    free(data);

    return ret;
}
