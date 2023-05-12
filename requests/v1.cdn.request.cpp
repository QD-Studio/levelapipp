#include "v1.cdn.request.h"

#include "../lapi_database.h"
#include <string>
#include <sys/stat.h>
#include <iostream>

#define file_exists(cstr) (stat(cstr, &buffer) == 0)

LevelAPI::v1::CDNRequest::CDNRequest() {
    this->request_name = "request data from server";
    this->request_url = "/api/v1/img/request/{file}";
}

std::shared_ptr<http_response> LevelAPI::v1::CDNRequest::render(const http_request &req) {
    auto file = std::string("images/") + std::string(req.get_arg("file"));
    //std::cout << file << std::endl;

    if (file.find("../") != std::string::npos || 
        file.find("/")   == 0                 || 
        file.find("~")   != std::string::npos  )
    {
        return sendFile("images/auto.png");
    }

    return sendFile(std::string(file));
}