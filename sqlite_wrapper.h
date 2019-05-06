#ifndef SQLITE_WRAPPER_H
#define SQLITE_WRAPPER_H
#include <sqlite3.h>
#include <string>
#include <queue>
#include <map>
#include <list>


#include "exceptions.h"

using Query = std::map<std::string, std::list<std::string>>;

class Sqlite_wrapper
{
    struct Column
    {
        std::string name;
        std::string type;
        std::string defaultValue;
        bool isPK;
        bool isUnique;
        bool isNullable;
        bool isDefaultValue;
        std::string getStatement();
        void clear();
    };
    struct ForeignKey
    {
        std::string column;
        std::string refTable;
        std::string refColumn;
        std::string getStatement();
        void clear();
    };

    struct Table
    {
        std::string name;
        std::queue<Column> columns;
        bool isForeignKey;
        std::queue<ForeignKey> foreignKeys;
        std::string getStatement();
        void clear();
    };
    static Query qResult;
    bool firstQuery;
    static int callback(void*, int argc, char **argv, char **azColName);//TODO
    const char *sqlite3Errmsg;
    Sqlite_wrapper();
    Sqlite_wrapper(const Sqlite_wrapper &other) = delete;
    Sqlite_wrapper(const Sqlite_wrapper &&other) = delete;
    Sqlite_wrapper& operator=(const Sqlite_wrapper &other) = delete;
    Sqlite_wrapper& operator=(const Sqlite_wrapper &&other) = delete;
    sqlite3 *base;
    Column curColumn;
    bool currentColumn;
    Table curTable;
    bool currentTable;
    void _createDatabase(const std::string &fileName);
    void _createTable(const std::string &tableName);//TODO
    void _createColumn(const std::string &columnName);//TODO
    void _setAsPK();//TODO
    void _setAsUnique();//TODO
    void _setAsNullable();//TODO
    void _setDefaultValue();//TODO
    void _addColumn();//TODO
    void _setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn);//TODO
    void _addTable();//TODO
    void _insert(const std::string &table, const std::list<std::string> &columns, const std::list<std::string> &values);//TODO
    void _select(const std::list<std::string> &columns, const std::string table);//TODO
    void _disconnectFromDatabase();//TODO

protected:

    virtual void Sqlite3ExceptionHandler(std::exception &e, const std::string &name = "");
    virtual void Sqlite3BusyExceptionHandler(std::exception &e, const std::string &name = "");
    virtual void CreateDatabaseExceptionHandler(std::exception &e);
public:
    static Sqlite_wrapper *connectToDatabase(const std::string &fileName);
    void createTable(const std::string &tableName);
    void createColumn(const std::string &columnName);
    void setAsPK();
    void setAsUnique();
    void setAsNullable();
    void setDefaultValue();
    void addColumn();
    void setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn = "");
    void addTable();
    void insert(const std::string &table, const std::list<std::string> &columns, const std::list<std::string> &values);
    Query select(const std::list<std::string> &columns, const std::string table);
    void disconnectFromDatabase();

    virtual ~Sqlite_wrapper();
};

#endif // SQLITE_WRAPPER_H
