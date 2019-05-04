#include "sqlite_wrapper.h"
#include <iostream>
Query qResult = Query();

Sqlite_wrapper::Sqlite_wrapper()
{
    firstQuery = false;
    currentTable = false;
    currentColumn = false;
    sqlite3Errmsg = nullptr;
}

void Sqlite_wrapper::_createDatabase(const std::string &fileName)
{
    if (fileName == "")
        throw CreateDatabaseException();
    int status;
    if ((status = sqlite3_open(fileName.c_str(), &base)))
        throw Sqlite3Exception(sqlite3_errmsg(base));
}

Sqlite_wrapper::~Sqlite_wrapper()
{

}

void Sqlite_wrapper::Sqlite3ExceptionHandler(std::exception &e, const std::string &name)
{
    std::cerr << "Error on " << name << ". " << e.what() << std::endl;
}

void Sqlite_wrapper::Sqlite3BusyExceptionHandler(std::exception &e, const std::string &name)
{

}

void Sqlite_wrapper::CreateDatabaseExceptionHandler(std::exception &e)
{
    std::cerr << e.what() << std::endl;
}

Sqlite_wrapper *Sqlite_wrapper::connectToDatabase(const std::string &fileName)
{
    Sqlite_wrapper *temp = new Sqlite_wrapper();
    try {
        temp->_createDatabase(fileName);
    } catch (std::exception &e) {
        temp->CreateDatabaseExceptionHandler(e);
        delete temp;
        return nullptr;
    }
    return temp;
}
