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

#include "lapi_database.h"

namespace LevelAPI {
    namespace NodeController {
        class NodeRunner {
        public:
            DatabaseController::Node *m_pLinkedNode;
            int m_nPreviousRequest;
            
            void processLinear();

            void errorReadOnly();
            void errorEmptyQueue();
        
            static void thread_downloader(NodeRunner *self);
            static void thread_resolver(NodeRunner *self);
            static void thread_cacheLevels(NodeRunner *self);
            static void thread_pushRecentTab(NodeRunner *self);
            
            static void task_waitRatelimit(NodeRunner *self, int rate_limit_length);

            void init(DatabaseController::Node *node);
        };
    }
}