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

#pragma once

#include "ServerModule.h"

#include <vector>
#include <string>

namespace LevelAPI {
    namespace DatabaseController {
        class Level;
    }

    namespace Backend {
        class ModuleGDHistory : public ServerModule {
        protected:
            // get gdhistory's api path
            std::string getAPIPath();
        public:
            // download archived level inside gdhistory
            virtual LevelAPI::DatabaseController::Level *downloadArchivedLevel(int id);
        };
    }
}
