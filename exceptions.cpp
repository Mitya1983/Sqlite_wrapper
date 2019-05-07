#include "exceptions.h"

Sqlite3Exception::Sqlite3Exception(const char *msg)
{
    _msg = msg;
}

Sqlite3Exception::Sqlite3Exception(std::string tableName, const char *msg)
{
    std::string message = "Error on table ";
    message += tableName;
    message += ": ";
    message += msg;
    _msg = message.c_str();
}

const char *Sqlite3Exception::what() const noexcept
{
    return _msg;
}

const char *Sqlite3BusyException::what() const noexcept
{
    return "Database is busy";
}

const char *CreateDatabaseException::what() const noexcept
{
    return "Database filename wasn't provided";
}

TableException::TableException(const std::string &tableName, const std::string &msg)
{
    std::string message = "Error on table ";
    message += tableName;
    message += ": ";
    message += msg;
    _msg = std::move(message);
}

const char *TableException::what() const noexcept
{
    return _msg.c_str();
}

ColumnException::ColumnException(const std::string &columnName, const std::string &msg)
{
    std::string message = "Error on column ";
    message += columnName;
    message += ": ";
    message += msg;
    _msg = std::move(message);
}

const char *ColumnException::what() const noexcept
{
    return _msg.c_str();
}

const char *InsertException::what() const noexcept
{

}
