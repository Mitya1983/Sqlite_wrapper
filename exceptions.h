#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>
#include <string>
class Sqlite3Exception : public std::exception
{
    const char *_msg;
public:
    Sqlite3Exception(const char *msg);
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
class CreateTableException : public std::exception
{
    const char *_msg;
public:
    CreateTableException(const std::string &tableName, const std::string &msg);
    virtual const char *what() const noexcept;
};
class CreateColumnException : public std::exception
{
public:
    virtual const char *what() const noexcept;
};
class PKorUniqueValueException : public std::exception
{
public:
    virtual const char *what() const noexcept;
};
class addColumnException : public std::exception
{
public:
    virtual const char *what() const noexcept;
};
class AddTableException : public std::exception
{
public:
    virtual const char *what() const noexcept;
};
class InsertException : std::exception
{
public:
    virtual const char *what() const noexcept;
};


#endif // EXCEPTIONS_H
