//
// Created by kirill on 10.12.2020.
//

#include "db.h"


int readCallback(void *context, int columns, char **data, char **names)
{
    auto records = (std::vector<Record> *)context;

    if (!columns) {
        std::cout << "Empty result!" << std::endl;
        return 0;
    }

    if (!names) {
        std::cout << "No columns!" << std::endl;
        return 0;
    }

    Record rec;
    rec.id = atoi(data[0]);
    rec.name = data[1];
    records->push_back(rec);

    if (!data) {
        std::cout << "No data!" << std::endl;
        return 0;
    }

    for (int i=0; i<columns; ++i) {
        if (i != 0)
            std::cout << ',';
        std::cout << data[i];
    }
    std::cout << std::endl;

    return 0;
}




std::string DB::open_db()
{
    std::string result = "OK";

    if (sqlite3_open(dbName.c_str(), &handle)) {
        result = std::string(errMsg);
        sqlite3_free(errMsg);
        isOpened = false;
        return result;
    }
    isOpened = true;
    return result;
}

std::string DB::insert_data(std::string command) {
    std::string result = "OK";
    if (!isOpened) {
        result = "ERR " + std::string(errMsg);
        sqlite3_free(errMsg);
        return result;
    }
    assert_tables_created();

    std::string insert;
    std::string table_name;
    std::string id;
    std::string name;

    std::istringstream command_as_stream(command);
    command_as_stream >> insert;
    command_as_stream >> table_name;
    command_as_stream >> id;
    command_as_stream >> name;
    std::string query_command = "INSERT INTO " + table_name + " VALUES(" + id + ",\'" + name + "\');";
    if (sqlite3_exec(handle, query_command.c_str(), /*callback*/nullptr, /*cb 1st argument*/nullptr, &errMsg)) {
        result = "ERR " + std::string(errMsg);
        sqlite3_free(errMsg);
        return result;
    }
    return result;
}

std::string DB::truncate_table(std::string command) {
    std::string result = "OK";
    if (!isOpened) {
        result = "ERR " + std::string(errMsg);
        sqlite3_free(errMsg);
        return result;
    }
    assert_tables_created();
    std::string truncate;
    std::string table_name;

    std::istringstream command_as_stream(command);
    command_as_stream >> truncate;
    command_as_stream >> table_name;
    std::string query_command = "DROP TABLE " + table_name;
    if (sqlite3_exec(handle, query_command.c_str(), /*callback*/nullptr, /*cb 1st argument*/nullptr, &errMsg)) {
        result = "ERR " + std::string(errMsg);
        sqlite3_free(errMsg);
        return result;
    }
    return result;
}

std::string DB::get_symmetric_difference()  {
    std::string result = "OK";
    if (!isOpened) {
        std::cout << "not opened " << errMsg << std::endl;
        result = "ERR " + std::string(errMsg);
        return result;
    }

    std::string query = "SELECT * FROM A WHERE A.id NOT IN (SELECT id FROM B) UNION SELECT * FROM B WHERE B.id NOT IN (SELECT id FROM A)";
    assert_tables_created();
    return select(query);
}

std::string DB::get_intersection() {
    std::string result = "OK";
    if (!isOpened) {
        result = "ERR " + std::string(errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    std::string query = "SELECT A.id, A.name, B.name FROM A INNER JOIN B ON A.id = B.id";
    assert_tables_created();
    return select(query);
}


std::string DB::select(std::string query)
{
    std::string result = "OK";
    std::vector<Record> records;

    if (sqlite3_exec(handle, query.c_str(), readCallback, &records, &errMsg)) {
        result = "ERR " + std::string(errMsg);
        sqlite3_free(errMsg);
    }
    return result;
}

std::string DB::assert_tables_created()
{
    std::string result = "OK";
    std::vector<std::string> create_requests{
            "CREATE TABLE IF NOT EXISTS A(id INTEGER PRIMARY KEY, name TEXT NOT NULL)",
            "CREATE TABLE IF NOT EXISTS B(id INTEGER PRIMARY KEY, name TEXT NOT NULL)"
    };
    for (auto & create_request: create_requests)
    {
        if (sqlite3_exec(handle, create_request.c_str(), nullptr, nullptr, &errMsg)) {
            result = "ERR " + std::string(errMsg);
            sqlite3_free(errMsg);
            return result;
        }
    }
    return result;
}

