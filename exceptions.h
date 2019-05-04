#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>
class Sqlite3Exception : public std::exception
{
    const char *_msg;
public:
    Sqlite3Exception(const char *msg);
    virtual const char *what() const noexcept;
};
class Sqlite3BusyException : public std::exception
{
    virtual const char *what() const noexcept;
};
class CreateDatabaseException : public std::exception
{
    virtual const char *what() const noexcept;
};
class CreateTableException : public std::exception
{
    virtual const char *what() const noexcept;
};
class CreateColumnException : public std::exception
{
    virtual const char *what() const noexcept;
};
class PKorUniqueValueException : public std::exception
{
    virtual const char *what() const noexcept;
};
class addColumnException : public std::exception
{
    virtual const char *what() const noexcept;
};
class AddTableException : public std::exception
{
    virtual const char *what() const noexcept;
};
class InsertException : std::exception
{
    virtual const char *what() const noexcept;
};


#endif // EXCEPTIONS_H
