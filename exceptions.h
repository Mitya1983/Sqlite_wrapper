#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>
#include <string>
class Sqlite3Exception : public std::exception
{
    const char *_msg;
public:
    Sqlite3Exception(const char *msg);
    Sqlite3Exception(std::string tableName, const char *msg);
    virtual const char *what() const noexcept;
};
class Sqlite3BusyException : public std::exception
{
public:
    virtual const char *what() const noexcept;
};
class CreateDatabaseException : public std::exception
{
public:
    virtual const char *what() const noexcept;
};
class TableException : public std::exception
{
    std::string _msg;
public:
    TableException(const std::string &tableName, const std::string &msg);
    virtual const char *what() const noexcept;
};
class ColumnException : public std::exception
{
    std::string _msg;
public:
    ColumnException(const std::string &columnName, const std::string &msg);
    virtual const char *what() const noexcept;
};
class InsertException : std::exception
{
public:
    virtual const char *what() const noexcept;
};


#endif // EXCEPTIONS_H
