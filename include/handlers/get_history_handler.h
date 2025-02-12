#ifndef GET_HISTORY_HANDLER_H
#define GET_HISTORY_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "util.h"
#include <cctype>
#include <sstream>
#include <iostream>


class GetHistoryHandler{
    public:
        static void get_history_handler(HttpRequest& req, HttpResponse& res, Database& db);
    private:
    static std::vector<std::vector<std::string>> get_table_content(const std::string& userId, Database& db);
    static std::string get_user_id(const std::string& token, Database& db);
};

#endif