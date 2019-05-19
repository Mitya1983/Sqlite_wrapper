#ifndef QUERY_H
#define QUERY_H
#include <string>
#include <vector>
using ParamVector = const std::vector<std::string>;
using ParamString = const std::string;
class InsertException : std::exception
{
    std::string _msg;
public:
    InsertException(const std::string &tableName, const std::string &msg);
    virtual const char *what() const noexcept;
};
class SelectException : public std::exception
{
    std::string _msg;
public:
    SelectException(const std::string &tableName, const std::string &msg);
    virtual const char *what() const noexcept;
};
class Query
{
private:
    std::string _query;
    std::string _table;
    bool select;
    void _insertInto(ParamString &table, ParamVector &columns, ParamVector &value);
    void _where(ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand);
    void _like(ParamVector &columnToCheck, ParamVector &like, ParamString &operand);
    void _glob(ParamVector &columnToCheck, ParamVector &glob, ParamString &operand);
    void _innerJoin(ParamString &table, ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand);
    void _outerJoin(ParamString &table, ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand);
    void _updateTable(ParamString &table, ParamVector &columns, ParamVector &values, ParamString &where);
protected:
    virtual void insertExceptionHandler(InsertException &e);
    virtual void selectFromExceptionHandler(SelectException &e);
public:
    Query();
    Query(const Query &other) = delete;
    Query& operator = (const Query &other) = delete;
    Query(const Query &&other) = delete;
    Query& operator = (const Query &&other) = delete;
    void insertInto(ParamString &table, ParamVector &columns, ParamVector &values);
    void selectFrom(ParamString table, ParamVector &columns);
    void where(ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand = ""/*and/or*/);
    void like(ParamVector &columnToCheck, ParamVector &like, ParamString &operand = ""/*and/or*/);
    void glob(ParamVector &columnToCheck, ParamVector &glob, ParamString &operand = ""/*and/or*/);
    void groupBy(ParamVector &columns);
    void orderBy(ParamVector &orderByColumn, ParamString &order = "asc");
    void limit(int limit);
    void crossJoin(ParamString &table);
    void innerJoin(ParamString &table, ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand = ""/*and/or*/);
    void outerJoin(ParamString &table, ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand = ""/*and/or*/);
    void updateTable(ParamString &table, ParamVector &columns, ParamVector &values, ParamString &where);
    void deleteRowFromTable(ParamString &table, ParamString &ID, ParamString &value);
    void clearTable(ParamString &table);
    std::string &query();
    virtual ~Query() = default;
};


#endif // QUERY_H
