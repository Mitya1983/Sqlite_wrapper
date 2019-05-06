#include "exceptions.h"

Sqlite3Exception::Sqlite3Exception(const char *msg)
{
    _msg = msg;
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

CreateTableException::CreateTableException(const std::string &tableName, const std::string &msg)
{
    std::string message = "Error on table ";
    message += tableName;
    message += ": ";
    message += msg;
    _msg = message.c_str();
}

const char *CreateTableException::what() const noexcept
{
    return _msg;
}

const char *CreateColumnException::what() const noexcept
{

}

const char *PKorUniqueValueException::what() const noexcept
{

}

const char *addColumnException::what() const noexcept
{

}

const char *AddTableException::what() const noexcept
{

}

const char *InsertException::what() const noexcept
{

}
