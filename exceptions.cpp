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

const char *CreateTableException::what() const noexcept
{

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
