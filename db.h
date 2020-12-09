//
// Created by kirill on 10.12.2020.
//

#pragma once
#include <string>

#include "boost/asio.hpp"
#include <boost/regex.hpp>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
#include <iostream>
#include "sqlite3.h"
#include <string>
#include <vector>


struct Record
{
    int id;
    std::string name;
};


int readCallback(void *context, int columns, char **data, char **names);


class DB
{
public:
    DB(std::string db_name) : dbName(db_name), handle(nullptr), errMsg(nullptr) {
        isOpened = false;
    }

    std::string open_db();
    std::string insert_data(std::string command);

    std::string truncate_table(std::string command);

    std::string get_symmetric_difference();

    std::string get_intersection();


private:
    std::string select(std::string query);
    std::string assert_tables_created();

    std::string dbName;
    sqlite3* handle;
    char* errMsg;
    bool isOpened;
};

