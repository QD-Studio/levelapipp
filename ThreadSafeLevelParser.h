#pragma once

#include "lapi_database.h"

namespace LevelAPI {
    namespace Backend {
        namespace LevelParser {
            LevelAPI::DatabaseController::Level *parseFromResponse(const char *response);
        }
    }
}