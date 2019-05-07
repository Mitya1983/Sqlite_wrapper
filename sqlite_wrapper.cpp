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

void Sqlite_wrapper::_noResultExec(const std::string query)
{
    int status;
    if ((status = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &sqlite3Errmsg)) != SQLITE_OK)
        throw Sqlite3Exception(sqlite3Errmsg);
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
    if ((status = sqlite3_open(path.c_str(), &db)))
        throw Sqlite3Exception(sqlite3_errmsg(db));
}

void Sqlite_wrapper::_createTable(const std::string &tableName)
{
    if (currentTable)
        throw TableException(tableName, "Work with previous table wasn't finished.\nPlease make sure that each createTable statement has corresponding addTable statement.");
    currentTable = true;
    curTable.name = std::move(tableName);
}

void Sqlite_wrapper::_createColumn(const std::string &columnName, const std::string &type)
{
    if (currentColumn)
        throw ColumnException(columnName, "Work with previous column wasn't finished.\nPlease make sure that each createColumn statement has corresponding addColumn statement.");
    currentColumn = true;
    curColumn.name = std::move(columnName);
    curColumn.type = std::move(type);
    curColumn.isPK = false;
    curColumn.isUnique = false;
    curColumn.isNullable = true;
    curColumn.isDefaultValue = false;
}

void Sqlite_wrapper::_setAsPK()
{
    if (curColumn.isDefaultValue)
        throw ColumnException(curColumn.name, "The default value was previosly set for this column.\nSetting it as Primary Key may provoke issues.");
    curColumn.isPK = true;
    curColumn.isUnique = true;
    curColumn.isNullable = false;
}

void Sqlite_wrapper::_setAsUnique()
{
    if (curColumn.isDefaultValue)
        throw ColumnException(curColumn.name, "The default value was previosly set for this column.\nSetting it as Unique may provoke issues.");
    curColumn.isUnique = true;
    curColumn.isNullable = false;
}

void Sqlite_wrapper::_setAsNotNullable()
{
    curColumn.isNullable = false;
}

void Sqlite_wrapper::_setDefaultValue(const std::string &value)
{
    if (curColumn.isPK || curColumn.isUnique)
        throw ColumnException(curColumn.name, "The column was previously set as Primery Key or Unique value.\n Setting a Default value may provoke issues.");
    curColumn.isDefaultValue = true;
    curColumn.defaultValue = std::move(value);
}

void Sqlite_wrapper::_addColumn()
{
    if (!currentColumn)
        throw ColumnException("undefined", "Work with column wasn't started.\ncreateColumn stetemnt should be used first");
    curTable.columns.push(std::move(curColumn));
    currentColumn = false;
    curColumn.clear();
}

void Sqlite_wrapper::_setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn)
{
    if (!curTable.isForeignKey)
        curTable.isForeignKey = true;
    ForeignKey key;
    key.column = std::move(column);
    key.refTable = std::move(refTable);
    if (refColumn == "")
        key.refColumn = column;
    else
        key.refColumn = std::move(refColumn);
    curTable.foreignKeys.push(std::move(key));
}

void Sqlite_wrapper::_addTable()
{
    if (!currentTable)
        throw TableException("undefined", "Work with table wasn't started.\ncreateTable stetemnt should be used first");
    _noResultExec(curTable.getStatement());
    curTable.clear();
    currentTable = false;
}

void Sqlite_wrapper::_insertInto(const std::string &table, const std::list<std::string> &columns, const std::list<std::string> &values)
{
    if (columns.size() != values.size())
        throw InsertException(table, "Columns and Values not correspond");
    std::string statement = "insert into ";
    statement += table;
    statement += " (";
    auto start = columns.begin();
    auto end = --columns.end();
    while (true)
    {
        if (start == end)
        {
            statement += *start;
            statement += ") ";
            break;
        }

        statement += *start;
        statement += ", ";
        start++;
    }
    statement += "values (";
    start = values.begin();
    end = --values.end();
    while (true)
    {
        statement += "'";
        statement += *start;
        if (start == end)
        {
            statement += "'); ";
            break;
        }
        statement += "', ";
        start++;
    }
    _noResultExec(statement);
}

void Sqlite_wrapper::_disconnectFromDatabase()
{
    int status;
    if ((status = sqlite3_close(db)) == SQLITE_BUSY)
        throw Sqlite3BusyException();
}

void Sqlite_wrapper::sqlite3ExceptionHandler(std::exception &e, const std::string &name)
{
    std::cerr << "Error from sqlite " << name << ": " << e.what() << std::endl;
    sqlite3_free(sqlite3Errmsg);
}

void Sqlite_wrapper::sqlite3BusyExceptionHandler(std::exception &e, const std::string &name)
{
    static int count = 1;
    std::cerr << e.what() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    try {
        count++;
        std::cerr << "Attempt to close connection to database #" << count << std::endl;
        _disconnectFromDatabase();
    } catch (std::exception &e) {
        sqlite3BusyExceptionHandler(e);
    }
}

void Sqlite_wrapper::createDatabaseExceptionHandler(std::exception &e)
{
    std::cerr << "createDatabase() Error " << e.what() << std::endl;
}

void Sqlite_wrapper::createTableExceptionHandler(std::exception &e)
{
    std::cerr << "createTable() Error" << e.what() << std::endl;
}

void Sqlite_wrapper::createColumnExceptionHandler(std::exception &e)
{
    std::cerr << "createColumn() Error " << e.what() << std::endl;
}

void Sqlite_wrapper::setPKExceptionHandler(std::exception &e)
{
    std::cerr << "setAsPK() Error " << e.what() << std::endl;
}

void Sqlite_wrapper::setUniqueExceptionHandler(std::exception &e)
{
    std::cerr << "setUnique() Error " << e.what() << std::endl;
}

void Sqlite_wrapper::setDefaultValueExceptionHandler(std::exception &e)
{
    std::cerr << "setDefaultValue() Error " << e.what() << std::endl;
}

void Sqlite_wrapper::addColumnExceptionHandler(std::exception &e)
{
    std::cerr << "addcolumn() Error " << e.what() << std::endl;
}

void Sqlite_wrapper::setForeignKeyExceptionHandler(std::exception &e)
{
    std::cerr << "setForeignKey() Error " << e.what() << std::endl;
}

void Sqlite_wrapper::addTableExceptionHandler(std::exception &e)
{
     std::cerr << "addTable() Error " << e.what() << std::endl;
     curTable.clear();
     currentTable = false;
}

void Sqlite_wrapper::insertExceptionHandler(std::exception &e)
{
    std::cerr << "insertInto() Error " << e.what() << std::endl;
}

Sqlite_wrapper *Sqlite_wrapper::connectToDatabase(const std::string &fileName)
{
    Sqlite_wrapper *temp = new Sqlite_wrapper();
    try {
        temp->_createDatabase(fileName);
    } catch (std::exception &e) {
        temp->createDatabaseExceptionHandler(e);
        delete temp;
        return nullptr;
    }
    return temp;
}

void Sqlite_wrapper::createTable(const std::string &tableName)
{
    try {
        _createTable(tableName);
    } catch (TableException &e) {
        createTableExceptionHandler(e);
    } catch (Sqlite3Exception &e) {
        sqlite3ExceptionHandler(e);
    }
}

void Sqlite_wrapper::createColumn(const std::string &columnName, const std::string &type)
{
    try {
        _createColumn(columnName, type);
    } catch (ColumnException &e) {
        createColumnExceptionHandler(e);
    }
}

void Sqlite_wrapper::setAsPK()
{
    try {
        _setAsPK();
    } catch (ColumnException &e) {
        setPKExceptionHandler(e);
    }
}

void Sqlite_wrapper::setAsUnique()
{
    try {
        _setAsUnique();
    } catch (ColumnException &e) {
        setUniqueExceptionHandler(e);
    }
}

void Sqlite_wrapper::setAsNotNullable()
{
    _setAsNotNullable();
}

void Sqlite_wrapper::setDefaultValue(const std::string &value)
{
    try {
        _setDefaultValue(value);
    } catch (ColumnException &e) {
        setDefaultValueExceptionHandler(e);
    }
}

void Sqlite_wrapper::addColumn()
{
    try {
        _addColumn();
    } catch (ColumnException &e) {
        addColumnExceptionHandler(e);
    }
}

void Sqlite_wrapper::setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn)
{
    try {
        _setForeinKey(column, refTable, refColumn);
    } catch (std::exception &e) {
        setForeignKeyExceptionHandler(e);
    }
}

void Sqlite_wrapper::addTable()
{
    try {
        _addTable();
    } catch (std::exception &e) {
        addTableExceptionHandler(e);
    }
}

void Sqlite_wrapper::insertInto(const std::string &table, const std::list<std::string> &columns, const std::list<std::string> &values)
{
    try {
        _insertInto(table, columns, values);
    } catch (std::exception &e) {
        insertExceptionHandler(e);
    }
}

Query Sqlite_wrapper::select(const std::list<std::string> &columns, const std::string table)
{

}

void Sqlite_wrapper::disconnectFromDatabase()
{
    try {
        _disconnectFromDatabase();
    } catch (std::exception &e) {
        sqlite3BusyExceptionHandler(e);
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
        statement += " default '";
        statement += defaultValue;
        statement += "'";
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
        throw TableException(name, "No columns were provided");

    std::string statement = "create table ";

    statement += name;
    statement += " (";
    while (columns.size() > 1)
    {
        statement += columns.front().getStatement();
        statement += ", ";
        columns.pop();

    }
    statement += columns.front().getStatement();
    columns.pop();
    if (isForeignKey)
    {
        statement += ", ";
        while (foreignKeys.size() > 1)
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
    while (columns.size() > 0)
        columns.pop();
    isForeignKey = false;
    while (foreignKeys.size() > 0)
        foreignKeys.pop();
}
