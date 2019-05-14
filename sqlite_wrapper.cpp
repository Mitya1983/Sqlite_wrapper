#include "sqlite_wrapper.h"
#include <iostream>
#include <chrono>
#include <thread>
std::shared_ptr<Query> Sqlite_wrapper::qResult = std::make_shared<Query>();
bool Sqlite_wrapper::firstQuery = true;
int Sqlite_wrapper::callback(void *, int argc, char **argv, char **azColName)
{
    if (firstQuery)
    {
        for (int i = 0; i < argc; i++)
        {
            qResult->emplace(azColName[i], std::vector<std::string>({argv[i] ? argv[i] : "NULL"}));
        }
        firstQuery = false;
    }
    else
        for (int i = 0; i < argc; i++)
        {
            qResult->at(azColName[i]).push_back(argv[i] ? argv[i] : "NULL");
        }
    return 0;
}

Sqlite_wrapper::Sqlite_wrapper()
{
    currentTable = false;
    currentColumn = false;
    sqlite3Errmsg = nullptr;
}

void Sqlite_wrapper::_modifyingExec(const std::string &query)
{
    int status;
    status = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &sqlite3Errmsg);
    while (status == SQLITE_BUSY)
    {
        sqlite3_free(sqlite3Errmsg);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        status = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &sqlite3Errmsg);
    }
    if (status != SQLITE_OK)
    {
        std::string msg = std::move(sqlite3Errmsg);
        sqlite3_free(sqlite3Errmsg);
        throw Sqlite3Exception(*name, query, msg);
    }
}

void Sqlite_wrapper::_readExec(const std::string &query)
{
    int status;
    auto _callback = Sqlite_wrapper::callback;
    if ((status = sqlite3_exec(db, query.c_str(), _callback, nullptr, &sqlite3Errmsg)) != SQLITE_OK)
    {
        std::string msg = std::move(sqlite3Errmsg);
        sqlite3_free(sqlite3Errmsg);
        throw Sqlite3Exception(*name, query, msg);
    }
}

void Sqlite_wrapper::_createDatabase(const std::string &fileName)
{
    if (fileName == "")
        throw CreateDatabaseException("Filename wasn't provided");
    std::string path = std::move(fileName);
    name = std::make_shared<std::string>(path.substr(path.find_last_of('/') + 1, path.length()));
    if (fileName.substr(fileName.length(), fileName.length() - 3) != ".db")
    {
        path += ".db";
    }
    int status;
    if ((status = sqlite3_open(path.c_str(), &db)))
        throw Sqlite3Exception(*name, path, sqlite3_errmsg(db));
}

void Sqlite_wrapper::_createTable(const std::string &table)
{
    if (currentTable)
        throw TableException(*name, table, "Work with previous table wasn't finished.\nPlease make sure that each createTable query has corresponding addTable query.");
    currentTable = true;
    curTable.name = std::move(table);
    curTable.databaseName = name;
    curTable.isForeignKey = false;
    curTable.noRowID = false;
    curTable.pKisSet = false;
}

void Sqlite_wrapper::_createColumn(const std::string &column, const std::string &type)
{
    if (currentColumn)
        throw ColumnException(*curTable.databaseName, curTable.name, column, "Work with previous column wasn't finished.\nPlease make sure that each createColumn query has corresponding addColumn query.");
    currentColumn = true;
    curColumn.name = std::move(column);
    curColumn.type = std::move(type);
    curColumn.isPK = false;
    curColumn.isUnique = false;
    curColumn.isNullable = true;
    curColumn.isDefaultValue = false;
}

void Sqlite_wrapper::_setAsPK()
{
    if (curColumn.isDefaultValue)
        throw ColumnException(*curTable.databaseName, curTable.name, curColumn.name, "The default value was previosly set for this column.\nSetting it as Primary Key may provoke issues.");
    curColumn.isPK = true;
    curColumn.isUnique = true;
    curColumn.isNullable = false;
    curTable.pKisSet = true;
}

void Sqlite_wrapper::_setAsUnique()
{
    if (curColumn.isDefaultValue)
        throw ColumnException(*curTable.databaseName, curTable.name, curColumn.name, "The default value was previosly set for this column.\nSetting it as Unique may provoke issues.");
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
        throw ColumnException(*curTable.databaseName, curTable.name, curColumn.name, "The column was previously set as Primery Key or Unique value.\n Setting a Default value may provoke issues.");
    curColumn.isDefaultValue = true;
    curColumn.defaultValue = std::move(value);
}

void Sqlite_wrapper::_setNoRowID()
{
    curTable.noRowID = true;
}

void Sqlite_wrapper::_addColumn()
{
    if (!currentColumn)
        throw ColumnException(*curTable.databaseName, curTable.name, "undefined", "Work with column wasn't started.\ncreateColumn stetemnt should be used first");
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
        throw TableException(*name, "undefined", "Work with table wasn't started.\ncreateTable stetemnt should be used first");
    _modifyingExec(curTable.getQuery());
    curTable.clear();
    currentTable = false;
}

void Sqlite_wrapper::_insertInto(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values)
{
    if (columns.size() != values.size())
        throw InsertException(*name, table, "Columns and Values not correspond");
    std::string query = "insert into ";
    query += table;
    query += " (";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        query += std::move(columns[i]);
        if (i < n - 1)
            query += ", ";
        else
            query += ") ";
    }
    query += "values (";
    for (unsigned int i = 0, n = values.size(); i < n; i++)
    {
        query += '\'' + std::move(values[i]) + '\'';
        if (i < n - 1)
            query += ", ";
        else
            query += ");";
    }
    _modifyingExec(query);
}

void Sqlite_wrapper::_selectFrom(const std::vector<std::string> &columns, const std::string table)
{
    qResult->clear();
    firstQuery = true;
    std::string query = "select ";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        query += std::move(columns[i]);
        if (i < n - 1)
            query += ", ";
        else
            query += " from ";
    }
    query += std::move(table);
    _readExec(query);
}

void Sqlite_wrapper::_selectFromOrderBy(const std::vector<std::string> &columns, const std::string table, const std::vector<std::string> &orderByColumn, const std::string &order)
{
    qResult->clear();
    firstQuery = true;
    std::string query = "select ";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        query += std::move(columns[i]);
        if (i < n - 1)
            query += ", ";
        else
            query += " from ";
    }
    query += std::move(table);
    query += " order by ";
    for (unsigned int i = 0, n = orderByColumn.size(); i < n; i++)
    {
        query += std::move(orderByColumn[i]);
        if (i < n - 1)
            query += ", ";
        else
            query += ' ';
    }
    query += std::move(order);
    _readExec(query);
}

void Sqlite_wrapper::_selectFromWhere(const std::vector<std::string> &columns, const std::string table,
                                      const std::vector<std::string> &where, const std::string &operand)
{
    qResult->clear();
    firstQuery = true;
    std::string query = "select ";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        query += std::move(columns[i]);
        if (i < n - 1)
            query += ", ";
        else
            query += " from ";
    }
    query += std::move(table);
    query += " where ";
    for (unsigned int i = 0, n = where.size(); i < n; i++)
    {
        query += std::move(where[i]);
        if (i < n - 1)
            query += ' ' + operand + ' ';
    }
    _readExec(query);
}

void Sqlite_wrapper::_getID(const std::string &IDName, const std::string &table, const std::string &columnName, const std::string &value)
{
    std::string query = "select ";
    query += std::move(IDName);
    query += " from ";
    query += std::move(table);
    query += " where ";
    query += std::move(columnName);
    query += " = ";
    query += '\'' + std::move(value) + '\'';

    _readExec(query);
}

void Sqlite_wrapper::_selectFromLike(const std::vector<std::string> &columns, const std::string &table,
                                     const std::vector<std::string> &columnToCheck, const std::vector<std::string> &like,
                                     const std::string & operand)
{
    if (columnToCheck.size() != like.size())
        throw SelectException(*name, table, "ColumnToCheck and Like statements not correspond");
    qResult->clear();
    firstQuery = true;
    std::string query = "select ";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        query += std::move(columns[i]);
        if (i < n - 1)
            query += ", ";
        else
            query += " from ";
    }
    query += std::move(table);
    query += " where ";
    for (unsigned int i = 0, n = columnToCheck.size(); i < n; i++)
    {
        query += std::move(columnToCheck[i]);
        query += " like '";
        query += std::move(like[i]);
        if (n > 1 && i < n - 1)
            query += "' " + operand + ' ';
        else
            query += '\'';
    }
    _readExec(query);
}

void Sqlite_wrapper::_selectFromGlob(const std::vector<std::string> &columns, const std::string &table, const std::vector<std::string> &columnToCheck, const std::vector<std::string> &glob, const std::string &operand)
{
    if (columnToCheck.size() != glob.size())
        throw SelectException(*name, table, "ColumnToCheck and Glob statements not correspond");
    qResult->clear();
    firstQuery = true;
    std::string query = "select ";
    for (unsigned int i = 0, n = columns.size(); i < n; i++)
    {
        query += std::move(columns[i]);
        if (i < n - 1)
            query += ", ";
        else
            query += " from ";
    }
    query += std::move(table);
    query += " where ";
    for (unsigned int i = 0, n = columnToCheck.size(); i < n; i++)
    {
        query += std::move(columnToCheck[i]);
        query += " glob '";
        query += std::move(glob[i]);
        if (n > 1 && i < n - 1)
            query += "' " + operand + ' ';
        else
            query += '\'';
    }
    _readExec(query);
}

void Sqlite_wrapper::_updateTable(const std::string &table, const std::vector<std::string> &columns,
                                  const std::vector<std::string> &values, const std::string &where)
{
    if (columns.size() != values.size())
        throw InsertException(*name, table, "Columns and Values not correspond");
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
    _modifyingExec(query);
}

void Sqlite_wrapper::_deleteRowFromTable(const std::string &table, const std::string &ID, const std::string &value)
{
    std::string query = "delete from ";
    query += std::move(table);
    query += " where ";
    query += std::move(ID);
    query += " = ";
    query += std::move(value);

    _modifyingExec(query);
}

void Sqlite_wrapper::_clearTable(const std::string &table)
{
    std::string query = "delete from ";
    query += std::move(table) + ';';

    _modifyingExec(query);
}

void Sqlite_wrapper::_disconnectFromDatabase()
{
    int status;
    if ((status = sqlite3_close(db)) == SQLITE_BUSY)
        throw Sqlite3Exception(*name, "", sqlite3_errmsg(db));
}

void Sqlite_wrapper::sqlite3ExceptionHandler(std::exception &e)
{
    std::cerr << e.what() << std::endl;
    sqlite3_free(sqlite3Errmsg);
}

void Sqlite_wrapper::sqlite3BusyExceptionHandler(std::exception &e)
{
    int count = 1;
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
    std::cerr << "createDatabase(): " << e.what() << std::endl;
}

void Sqlite_wrapper::createTableExceptionHandler(std::exception &e)
{
    std::cerr << "createTable(): " << e.what() << std::endl;
}

void Sqlite_wrapper::createColumnExceptionHandler(std::exception &e)
{
    std::cerr << "createColumn(): " << e.what() << std::endl;
}

void Sqlite_wrapper::setPKExceptionHandler(std::exception &e)
{
    std::cerr << "setAsPK(): " << e.what() << std::endl;
}

void Sqlite_wrapper::setUniqueExceptionHandler(std::exception &e)
{
    std::cerr << "setUnique(): " << e.what() << std::endl;
}

void Sqlite_wrapper::setDefaultValueExceptionHandler(std::exception &e)
{
    std::cerr << "setDefaultValue(): " << e.what() << std::endl;
}

void Sqlite_wrapper::addColumnExceptionHandler(std::exception &e)
{
    std::cerr << "addcolumn(): " << e.what() << std::endl;
}

void Sqlite_wrapper::setForeignKeyExceptionHandler(std::exception &e)
{
    std::cerr << "setForeignKey(): " << e.what() << std::endl;
}

void Sqlite_wrapper::addTableExceptionHandler(std::exception &e)
{
     std::cerr << "addTable(): " << e.what() << std::endl;
     curTable.clear();
     currentTable = false;
}

void Sqlite_wrapper::insertExceptionHandler(std::exception &e)
{
    std::cerr << "insertInto(): " << e.what() << std::endl;
}

void Sqlite_wrapper::selectFromExceptionHandler(std::exception &e)
{
    std::cerr << "selectFrom(): " << e.what() << std::endl;
}

void Sqlite_wrapper::updateExceptionHandler(std::exception &e)
{
    std::cerr << "updateTable(): " << e.what() << std::endl;
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

void Sqlite_wrapper::createTable(const std::string &table)
{
    try {
        _createTable(table);
    } catch (TableException &e) {
        createTableExceptionHandler(e);
    } catch (Sqlite3Exception &e) {
        sqlite3ExceptionHandler(e);
    }
}

void Sqlite_wrapper::createColumn(const std::string &column, const std::string &type)
{
    try {
        _createColumn(column, type);
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

void Sqlite_wrapper::insertInto(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values)
{
    try {
        _insertInto(table, columns, values);
    } catch (std::exception &e) {
        insertExceptionHandler(e);
    }
}

void Sqlite_wrapper::printToShell(std::shared_ptr<Query> result)
{
    if (result->size() == 0)
    {
        std::cout << "No rows were selected" << std::endl;
        return;
    }
    auto start = result->begin();
    auto end = result->end();
    unsigned int rowCount = 0;
    while (true)
    {
        if (start == end)
        {
            std::cout << '\n';
            break;
        }
        if (rowCount < start->second.size())
            rowCount = start->second.size();
        std::cout << start->first << "\t|\t";
        start++;
    }
    for (unsigned int i = 0; i < rowCount; i++)
    {
        if (start == end)
            start = result->begin();
        while (true)
        {
            if (start == end)
                break;
            std::cout << start->second[i] << "\t|\t";
            start++;
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

std::shared_ptr<Query> Sqlite_wrapper::selectFrom(const std::vector<std::string> &columns, const std::string table)
{
    try {
        _selectFrom(columns, table);
    } catch (Sqlite3Exception &e) {
        selectFromExceptionHandler(e);
    }
    return qResult;
}

std::shared_ptr<Query> Sqlite_wrapper::selectFromOrderBy(const std::vector<std::string> &columns, const std::string table,
                                                         const std::vector<std::string> &orderByColumn, const std::string &order)
{
    try {
        _selectFromOrderBy(columns, table, orderByColumn, order);
    } catch (std::exception &e) {
        sqlite3ExceptionHandler(e);
    }
    return qResult;
}

std::shared_ptr<Query> Sqlite_wrapper::selectFromWhere(const std::vector<std::string> &columns, const std::string table,
                                                       const std::vector<std::string> &where, const std::string &operand)
{
    try {
        _selectFromWhere(columns, table, where, operand);
    } catch (Sqlite3Exception &e) {
        selectFromExceptionHandler(e);
    }
    return qResult;
}

std::shared_ptr<Query> Sqlite_wrapper::selectFromLike(const std::vector<std::string> &columns, const std::string &table,
                                                      const std::vector<std::string> &columnToCheck, const std::vector<std::string> &like,
                                                      const std::string & operand)
{
    try {
        _selectFromLike(columns, table, columnToCheck, like, operand);
    } catch (Sqlite3Exception &e) {
        selectFromExceptionHandler(e);
    }
    return qResult;
}

std::shared_ptr<Query> Sqlite_wrapper::selectFromGlob(const std::vector<std::string> &columns, const std::string &table, const std::vector<std::string> &columnToCheck, const std::vector<std::string> &glob, const std::string &operand)
{
    try {
        _selectFromGlob(columns, table, columnToCheck, glob, operand);
    } catch (Sqlite3Exception &e) {
        selectFromExceptionHandler(e);
    }
    return qResult;
}

std::string Sqlite_wrapper::getID(const std::string &table, const std::string &columnName, const std::string &value, const std::string &IDName)
{
    std::string _IDName;
    if (IDName == "")
        _IDName = std::move(table) + "ID";
    else
        _IDName = std::move(IDName);
    try {
        _getID(_IDName, table, columnName, value);
    } catch (std::exception &e) {
        sqlite3ExceptionHandler(e);
    }
    if (qResult->size() == 0)
        return "0";

    return qResult->begin()->second[0];
}

void Sqlite_wrapper::updateTable(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values, const std::string &where)
{
    try {
        _updateTable(table, columns, values, where);
    } catch (std::exception &e) {
        updateExceptionHandler(e);
    }
}

void Sqlite_wrapper::deleteRowFromTable(const std::string &table, const std::string &ID, const std::string &value)
{
    try {
        _deleteRowFromTable(table, ID, value);
    } catch (std::exception &e) {
        sqlite3ExceptionHandler(e);
    }
}

void Sqlite_wrapper::clearTable(const std::string &table)
{
    try {
        _clearTable(table);
    } catch (std::exception &e) {
        sqlite3ExceptionHandler(e);
    }
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

std::string Sqlite_wrapper::Column::getQuery()
{
    std::string query = name;

    query += " ";
    query += type;
    if (isPK)
        query += " primary key";
    if (isUnique)
        query += " unique";
    if (isNullable)
        query += " null";
    else
        query += " not null";
    if (isDefaultValue)
    {
        query += " default '";
        query += defaultValue;
        query += "'";
    }

    return query;
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

std::string Sqlite_wrapper::ForeignKey::getQuery()
{
    std::string query = "foreign key (";

    query += column;
    query += " ) references ";
    query += refTable;
    query += " (";
    query += refColumn;
    query += ")";

    return query;
}

void Sqlite_wrapper::ForeignKey::clear()
{
    column.clear();
    refTable.clear();
    refColumn.clear();
}

std::string Sqlite_wrapper::Table::getQuery()
{
    if (columns.size() == 0)
        throw TableException(*databaseName, name, "No columns were provided");

    std::string query = "create table ";

    query += name;
    query += " (";
    if (!pKisSet && noRowID)
        query += name + "ID integer not null unique, ";
    while (columns.size() > 1)
    {
        query += columns.front().getQuery();
        query += ", ";
        columns.pop();
    }
    query += columns.front().getQuery();
    columns.pop();
    if (isForeignKey)
    {
        query += ", ";
        while (foreignKeys.size() > 1)
        {
            query += foreignKeys.front().getQuery();
            query += ", ";
            foreignKeys.pop();
        }
        query += foreignKeys.front().getQuery();
        foreignKeys.pop();
    }
    if (noRowID)
        query += ") without rowid;";
    else
        query += ");";

    return query;
}

void Sqlite_wrapper::Table::clear()
{
    name.clear();
    while (columns.size() > 0)
        columns.pop();
    isForeignKey = false;
    while (foreignKeys.size() > 0)
        foreignKeys.pop();
    noRowID = false;
    pKisSet = false;
}
