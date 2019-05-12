#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>
#include <string>
//TODO: Reimplement all Exceptions same as SqliteException
class Sqlite3Exception : public std::exception
{
    std::string _msg;
public:
    Sqlite3Exception(const std::string &databaseName, const std::string &details, const std::string &msg);
    virtual const char *what() const noexcept;
};

class CreateDatabaseException : public std::exception
{
    std::string _msg;
public:
    CreateDatabaseException(const std::string &msg);
    virtual const char *what() const noexcept;
};
class TableException : public std::exception
{
    std::string _msg;
public:
    TableException(const std::string &databaseName, const std::string &tableName, const std::string &msg);
    virtual const char *what() const noexcept;
};
class ColumnException : public std::exception
{
    std::string _msg;
public:
    ColumnException(const std::string &databaseName, const std::string &tableName,
                    const std::string &columnName, const std::string &msg);
    virtual const char *what() const noexcept;
};
class InsertException : std::exception
{
    std::string _msg;
public:
    InsertException(const std::string &databaseName, const std::string &tableName, const std::string &msg);
    virtual const char *what() const noexcept;
};
class SelectException : public std::exception
{
    std::string _msg;
public:
    SelectException(const std::string &databaseName, const std::string &tableName, const std::string &msg);
    virtual const char *what() const noexcept;
};


#endif // EXCEPTIONS_H
