#include "sqlite_wrapper.h"
#include <iostream>
#include <chrono>
#include <thread>
Query qResult = Query();

Sqlite_wrapper::Sqlite_wrapper()
{
    firstQuery = false;
    currentTable = false;
    currentColumn = false;
    sqlite3Errmsg = nullptr;

}

void Sqlite_wrapper::_createDatabase(const std::string &fileName)
{
    if (fileName == "")
        throw CreateDatabaseException();
    std::string path = std::move(fileName);
    if (fileName.substr(fileName.length(), fileName.length() - 3) != ".db")
    {
        path += ".db";
    }
    int status;
    if ((status = sqlite3_open(path.c_str(), &base)))
        throw Sqlite3Exception(sqlite3_errmsg(base));
}

void Sqlite_wrapper::_createTable(const std::string &tableName)
{
    if (currentTable)
        throw CreateTableException(tableName, "Work with previous table wasn't finished.\nPlease make sure that each \
                                               createTable has corresponding addTable statement");
}

void Sqlite_wrapper::_disconnectFromDatabase()
{
    int status;
    if ((status = sqlite3_close(base)) == SQLITE_BUSY)
        throw Sqlite3BusyException();
}

void Sqlite_wrapper::Sqlite3ExceptionHandler(std::exception &e, const std::string &name)
{
    std::cerr << "Error on " << name << ". " << e.what() << std::endl;
}

void Sqlite_wrapper::Sqlite3BusyExceptionHandler(std::exception &e, const std::string &name)
{
    static int count = 1;
    std::cerr << e.what() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    try {
        count++;
        std::cerr << "Attempt to close connection to database #" << count << std::endl;
        _disconnectFromDatabase();
    } catch (std::exception &e) {
        Sqlite3BusyExceptionHandler(e);
    }
}

void Sqlite_wrapper::CreateDatabaseExceptionHandler(std::exception &e)
{
    std::cerr << e.what() << std::endl;
}

Sqlite_wrapper *Sqlite_wrapper::connectToDatabase(const std::string &fileName)
{
    Sqlite_wrapper *temp = new Sqlite_wrapper();
    try {
        temp->_createDatabase(fileName);
    } catch (std::exception &e) {
        temp->CreateDatabaseExceptionHandler(e);
        delete temp;
        return nullptr;
    }
    return temp;
}

void Sqlite_wrapper::createTable(const std::string &tableName)
{

}

void Sqlite_wrapper::createColumn(const std::string &columnName)
{

}

void Sqlite_wrapper::setAsPK()
{

}

void Sqlite_wrapper::setAsUnique()
{

}

void Sqlite_wrapper::setAsNullable()
{

}

void Sqlite_wrapper::setDefaultValue()
{

}

void Sqlite_wrapper::addColumn()
{

}

void Sqlite_wrapper::setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn)
{

}

void Sqlite_wrapper::addTable()
{

}

void Sqlite_wrapper::insert(const std::string &table, const std::list<std::string> &columns, const std::list<std::string> &values)
{

}

Query Sqlite_wrapper::select(const std::list<std::string> &columns, const std::string table)
{

}

void Sqlite_wrapper::disconnectFromDatabase()
{
    try {
        _disconnectFromDatabase();
    } catch (std::exception &e) {
        Sqlite3BusyExceptionHandler(e);
    }
}

Sqlite_wrapper::~Sqlite_wrapper()
{
    disconnectFromDatabase();
}

std::string Sqlite_wrapper::Column::getStatement()
{
    std::string statement = name;

    statement += " ";
    statement += type;
    if (isPK)
        statement += " primary key";
    if (isUnique)
        statement += " unique";
    if (isNullable)
        statement += " null";
    else
        statement += " not null";
    if (isDefaultValue)
    {
        statement += " default ";
        statement += defaultValue;
    }

    return statement;
}

void Sqlite_wrapper::Column::clear()
{
    name.clear();
    type.clear();
    defaultValue.clear();
    isPK = false;
    isUnique = false;
    isNullable = true;
    isDefaultValue = false;
}

std::string Sqlite_wrapper::ForeignKey::getStatement()
{
    std::string statement = "foreign key (";

    statement += column;
    statement += " ) references ";
    statement += refTable;
    statement += " (";
    statement += refColumn;
    statement += ")";

    return statement;
}

void Sqlite_wrapper::ForeignKey::clear()
{
    column.clear();
    refTable.clear();
    refColumn.clear();
}

std::string Sqlite_wrapper::Table::getStatement()
{
    if (columns.size() == 0)
        throw CreateTableException(name, "No columns were provided");

    std::string statement = "create table ";

    statement += name;
    statement += " (";
    while (columns.size() > 0)
    {
        statement += columns.front().getStatement();
        statement += ", ";
        columns.pop();

    }
    statement += columns.front().getStatement();
    columns.pop();
    if (isForeignKey)
    {
        while (foreignKeys.size() > 0)
        {
            statement += foreignKeys.front().getStatement();
            statement += ", ";
            foreignKeys.pop();
        }
        statement += foreignKeys.front().getStatement();
        foreignKeys.pop();
    }
    statement += ");";

    return statement;
}

void Sqlite_wrapper::Table::clear()
{
    name.clear();
    while (sizeof (columns) != 0)
        columns.pop();
    isForeignKey = false;
    while (sizeof (foreignKeys) != 0)
        foreignKeys.pop();
}
