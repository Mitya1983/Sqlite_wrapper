#include "query.h"

#include <iostream>

InsertException::InsertException(const std::string &tableName, const std::string &msg)
{
    _msg = "Insert Error on table ";
    _msg += std::move(tableName);
    _msg += ": ";
    _msg += std::move(msg);
}

const char *InsertException::what() const noexcept
{
    return _msg.c_str();
}

SelectException::SelectException(const std::string &tableName, const std::string &msg)
{
    _msg = "Select Error on table ";
    _msg += std::move(tableName);
    _msg += ": ";
    _msg += std::move(msg);
}

const char *SelectException::what() const noexcept
{
    return _msg.c_str();
}

Query::Query()
{
    select = false;
}

void Query::_insertInto(ParamString &table, ParamVector &columns, ParamVector &value)
{
    if (columns.size() != value.size())
        throw InsertException(table, "Columns and Values not correspond");
    _query.clear();
    _query = "insert into ";
    _query += table;
    _query += " (";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        _query += std::move(columns[i]);
        if (i < n - 1)
            _query += ", ";
        else
            _query += ") ";
    }
    _query += "values (";
    for (unsigned int i = 0, n = value.size(); i < n; i++)
    {
        _query += '\'' + std::move(value[i]) + '\'';
        if (i < n - 1)
            _query += ", ";
        else
            _query += ");";
    }
}

void Query::_where(ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand)
{
    if (!select)
        throw SelectException("", "selectFrom() function wasn't previously invoked.");
    if (columnToCheck.size() != compareOperator.size() || compareOperator.size() != value.size())
        throw SelectException(_table, "ColumnsToCheck, Compare and Values statements not correspond");
    _query.erase(_query.find_last_of(';'));
    if (_query.find("where") != std::string::npos)
        _query += " and ";
    else
        _query += " where ";
    for (unsigned int i = 0, n = columnToCheck.size(); i < n; i++)
    {
        _query += std::move(columnToCheck[i]);
        _query += ' ' + std::move(compareOperator[i]) + ' ';
        if (value[i].find('.') == std::string::npos)
            _query += '\'' + std::move(value[i]) + '\'';
        else
            _query += std::move(value[i]);
        if (i < n - 1)
            _query += ' ' + operand + ' ';
        else
            _query += ';';
    }
}

void Query::_like(ParamVector &columnToCheck, ParamVector &like, ParamString &operand)
{
    if (!select)
        throw SelectException("", "selectFrom() function wasn't previously invoked.");
    if (columnToCheck.size() != like.size())
        throw SelectException(_table, "ColumnsToCheck and Like statements not correspond");
    _query.erase(_query.find_last_of(';'));
    if (_query.find("where") != std::string::npos)
        _query += " and ";
    else
        _query += " where ";
    for (unsigned int i = 0, n = columnToCheck.size(); i < n; i++)
    {
        _query += std::move(columnToCheck[i]);
        _query += " like '";
        _query += std::move(like[i]);
        if (i < n - 1)
            _query += "' " + operand + ' ';
        else
            _query += "';";
    }
}

void Query::_glob(ParamVector &columnToCheck, ParamVector &glob, ParamString &operand)
{
    if (!select)
        throw SelectException("", "selectFrom() function wasn't previously invoked.");
    if (columnToCheck.size() != glob.size())
        throw SelectException(_table, "ColumnsToCheck and Glob statements not correspond");
    _query.erase(_query.find_last_of(';'));
    if (_query.find("where") != std::string::npos)
        _query += " and ";
    else
        _query += " where ";
    for (unsigned int i = 0, n = columnToCheck.size(); i < n; i++)
    {
        _query += std::move(columnToCheck[i]);
        _query += " glob '";
        _query += std::move(glob[i]);
        if (i < n - 1)
            _query += "' " + operand + ' ';
        else
            _query += "';";
    }
}

void Query::_innerJoin(ParamString &table, ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand)
{
    if (!select)
        throw SelectException("", "selectFrom() function wasn't previously invoked.");
    if (columnToCheck.size() != compareOperator.size() || compareOperator.size() != value.size())
        throw SelectException(_table, "ColumnsToCheck, Compare and Values statements not correspond");
    _query.erase(_query.find_last_of(';'));
    _query += " inner join ";
    _query += std::move(table);
    _query += " on ";
    for (unsigned int i = 0, n = columnToCheck.size(); i < n; i++)
    {
        _query += std::move(columnToCheck[i]);
        _query += ' ' + std::move(compareOperator[i]) + ' ';
        if (value[i].find('.') == std::string::npos)
            _query += '\'' + std::move(value[i]) + '\'';
        else
            _query += std::move(value[i]);
        if (i < n - 1)
            _query += ' ' + operand + ' ';
        else
            _query += ';';
    }
}

void Query::_outerJoin(ParamString &table, ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand)
{
    if (!select)
        throw SelectException("", "selectFrom() function wasn't previously invoked.");
    if (columnToCheck.size() != compareOperator.size() || compareOperator.size() != value.size())
        throw SelectException(_table, "ColumnsToCheck, Compare and Values statements not correspond");
    _query.erase(_query.find_last_of(';'));
    _query += " left outer join ";
    _query += std::move(table);
    _query += " on ";
    for (unsigned int i = 0, n = columnToCheck.size(); i < n; i++)
    {
        _query += std::move(columnToCheck[i]);
        _query += ' ' + std::move(compareOperator[i]) + ' ';
        if (value[i].find('.') == std::string::npos)
            _query += '\'' + std::move(value[i]) + '\'';
        else
            _query += std::move(value[i]);
        if (i < n - 1)
            _query += ' ' + operand + ' ';
        else
            _query += ';';
    }
}

void Query::_updateTable(ParamString &table, ParamVector &columns, ParamVector &values, ParamString &where)
{
    if (columns.size() != values.size())
        throw InsertException(table, "Columns and Values not correspond");
    std::string query = "update ";
    query += std::move(table);
    query += " set ";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        query += std::move(columns[i]);
        query += " = \'";
        query += std::move(values[i]);
        query += "\'";
        if (i < n - 1)
            query += ", ";
    }
    query += " where ";
    query += std::move(where);
}

void Query::insertExceptionHandler(InsertException &e)
{
    std::cerr << e.what() << std::endl;
}

void Query::selectFromExceptionHandler(SelectException &e)
{
    std::cerr << e.what() << std::endl;
}

void Query::insertInto(ParamString &table, ParamVector &columns, ParamVector &values)
{
    try {
        _insertInto(table, columns, values);
    } catch (InsertException &e) {

    }
}

void Query::selectFrom(ParamString table, ParamVector &columns)
{
    _query.clear();
    _table = table;
    select = true;
    _query = "select ";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        _query += std::move(columns[i]);
        if (i < n - 1)
            _query += ", ";
        else
            _query += " from ";
    }
    _query += std::move(table) + ';';
}

void Query::where(ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand)
{
    try {
        _where(columnToCheck, compareOperator, value, operand);
    } catch (SelectException &e) {
        selectFromExceptionHandler(e);
    }
}

void Query::like(ParamVector &columnToCheck, ParamVector &like, ParamString &operand)
{
    try {
        _like(columnToCheck, like, operand);
    } catch (SelectException &e) {
        selectFromExceptionHandler(e);
    }
}

void Query::glob(ParamVector &columnToCheck, ParamVector &glob, ParamString &operand)
{
    try {
        _glob(columnToCheck, glob, operand);
    } catch (SelectException &e) {
        selectFromExceptionHandler(e);
    }
}

void Query::groupBy(ParamVector &columns)
{
    _query.erase(_query.find_last_of(';'));
    _query += " group by ";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        _query += std::move(columns[i]);
        if (i < n - 1)
            _query += ", ";
        else
            _query += ';';
    }
}

void Query::orderBy(ParamVector &orderByColumn, ParamString &order)
{
    _query.erase(_query.find_last_of(';'));
    _query += " order by ";
    for (unsigned int i = 0, n = orderByColumn.size(); i < n; i++)
    {
        _query += std::move(orderByColumn[i]);
        if (i < n - 1)
            _query += ", ";
        else
            _query += ' ';
    }
    _query += std::move(order) + ';';
}

void Query::limit(int limit)
{
    _query.erase(_query.find_last_of(';'));
    _query += " limit ";
    _query += std::to_string(limit) + ';';
}

void Query::crossJoin(ParamString &table)
{
    _query.erase(_query.find_last_of(';'));
    _query += " cross join ";
    _query += std::move(table) + ';';
}

void Query::innerJoin(ParamString &table, ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand)
{
    try {
        _innerJoin(table, columnToCheck, compareOperator, value, operand);
    } catch (SelectException &e) {
        selectFromExceptionHandler(e);
    }
}

void Query::outerJoin(ParamString &table, ParamVector &columnToCheck, ParamVector &compareOperator, ParamVector &value, ParamString &operand)
{
    try {
        _outerJoin(table, columnToCheck, compareOperator, value, operand);
    } catch (SelectException &e) {
        selectFromExceptionHandler(e);
    }
}

void Query::updateTable(ParamString &table, ParamVector &columns, ParamVector &values, ParamString &where)
{
    try {
        _updateTable(table, columns, values, where);
    } catch (InsertException &e) {
        insertExceptionHandler(e);
    }
}

void Query::deleteRowFromTable(ParamString &table, ParamString &ID, ParamString &value)
{
    _query.clear();
    _query = "delete from ";
    _query += std::move(table);
    _query += " where ";
    _query += std::move(ID);
    _query += " = ";
    _query += std::move(value);
}

void Query::clearTable(ParamString &table)
{
    _query.clear();
    _query = "delete from ";
    _query += std::move(table) + ';';
}

std::string &Query::toString()
{
    select = false;
    return _query;
}



