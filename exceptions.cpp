#include "exceptions.h"

Sqlite3Exception::Sqlite3Exception(const std::string &databaseName, const std::string &details, const std::string &msg)
{
    _msg = "Error from SQL on database ";
    _msg += databaseName;
    _msg += ": On ";
    _msg += details;
    _msg += "- ";
    _msg += std::move(msg);
}

const char *Sqlite3Exception::what() const noexcept
{
    return _msg.c_str();
}

CreateDatabaseException::CreateDatabaseException(const std::string &msg)
{
    _msg = std::move(msg);
}

const char *CreateDatabaseException::what() const noexcept
{
    return _msg.c_str();
}

TableException::TableException(const std::string &databaseName, const std::string &tableName, const std::string &msg)
{
    _msg = "Error on table ";
    _msg += tableName;
    _msg += " of database ";
    _msg += databaseName;
    _msg += ": ";
    _msg += std::move(msg);
}

const char *TableException::what() const noexcept
{
    return _msg.c_str();
}

ColumnException::ColumnException(const std::string &databaseName, const std::string &tableName,
                                 const std::string &columnName, const std::string &msg)
{
    _msg = "Error on column ";
    _msg += columnName;
    _msg += " of table ";
    _msg += tableName;
    _msg += " of databse ";
    _msg += databaseName;
    _msg += ": ";
    _msg += std::move(msg);
}

const char *ColumnException::what() const noexcept
{
    return _msg.c_str();
}

InsertException::InsertException(const std::string &databaseName, const std::string &tableName, const std::string &msg)
{
    _msg = "Error on table ";
    _msg += tableName;
    _msg += " of databse ";
    _msg += databaseName;
    _msg += ": ";
    _msg += std::move(msg);
}

const char *InsertException::what() const noexcept
{
    return _msg.c_str();
}

SelectException::SelectException(const std::string &databaseName, const std::string &tableName, const std::string &msg)
{
    _msg = "Error on table ";
    _msg += tableName;
    _msg += " of databse ";
    _msg += databaseName;
    _msg += ": ";
    _msg += std::move(msg);
}

const char *SelectException::what() const noexcept
{
    return _msg.c_str();
}
