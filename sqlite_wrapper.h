#ifndef SQLITE_WRAPPER_H
#define SQLITE_WRAPPER_H
#include <sqlite3.h>
#include <string>
#include <queue>
#include <map>
#include <vector>
#include <memory>

#include "exceptions.h"
//TODO Implement that ID name automaticaly shoud be set to Table Name + ID if no primary key provided.
using Query = std::map<std::string, std::vector<std::string>>;

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
        std::string getQuery();
        void clear();
    };
    struct ForeignKey
    {
        std::string column;
        std::string refTable;
        std::string refColumn;
        std::string getQuery();
        void clear();
    };

    struct Table
    {
        std::shared_ptr<std::string> databaseName;
        std::string name;
        std::queue<Column> columns;
        bool pKisSet;
        bool isForeignKey;
        std::queue<ForeignKey> foreignKeys;
        bool noRowID;
        std::string getQuery();
        void clear();
    };
    std::shared_ptr<std::string> name;
    static std::shared_ptr<Query> qResult;
    static bool firstQuery;
    static int callback(void*, int argc, char **argv, char **azColName);
    char *sqlite3Errmsg;
    Sqlite_wrapper();
    Sqlite_wrapper(const Sqlite_wrapper &other) = delete;
    Sqlite_wrapper(const Sqlite_wrapper &&other) = delete;
    Sqlite_wrapper& operator=(const Sqlite_wrapper &other) = delete;
    Sqlite_wrapper& operator=(const Sqlite_wrapper &&other) = delete;
    sqlite3 *db;
    Column curColumn;
    bool currentColumn;
    Table curTable;
    bool currentTable;
    void _modifyingExec(const std::string &query);
    void _readExec(const std::string &query);
    void _createDatabase(const std::string &fileName);
    void _createTable(const std::string &table);
    void _createColumn(const std::string &column, const std::string &type);
    void _setAsPK();
    void _setAsUnique();
    void _setAsNotNullable();
    void _setDefaultValue(const std::string &value);
    void _setNoRowID();//If Primary Key is not set the [Table name]ID column will be created with INTEGER type. Autoincrement willnot work
    void _addColumn();
    void _setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn);
    void _addTable();
    void _dropTable(const std::string &table);//TODO
    void _insertInto(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values);
    void _selectFrom(const std::vector<std::string> &columns, const std::string table);
    void _selectFromOrderBy(const std::vector<std::string> &columns, const std::string table,
                            const std::vector<std::string> &orderByColumn, const std::string &order);
    void _selectFromWhere(const std::vector<std::string> &columns, const std::string table,
                          const std::vector<std::string> &where, const std::string &operand);
    void _getID(const std::string &IDName, const std::string &table, const std::string &columnName, const std::string &value);
    void _selectFromLike(const std::vector<std::string> &columns, const std::string &table,
                         const std::vector<std::string> &columnToCheck, const std::vector<std::string> &like,
                         const std::string & operand);
    void _selectFromGlob(const std::vector<std::string> &columns, const std::string &table,
                         const std::vector<std::string> &columnToCheck, const std::vector<std::string> &glob,
                         const std::string & operand);
    void _updateTable(const std::string &table, const std::vector<std::string> &columns,
                      const std::vector<std::string> &values, const std::string &where);
    void _deleteRowFromTable(const std::string &table, const std::string &ID, const std::string &value);
    void _clearTable(const std::string &table);
    void _disconnectFromDatabase();

protected:
    virtual void sqlite3ExceptionHandler(std::exception &e);
    virtual void sqlite3BusyExceptionHandler(std::exception &e);
    virtual void createDatabaseExceptionHandler(std::exception &e);
    virtual void createTableExceptionHandler(std::exception &e);
    virtual void createColumnExceptionHandler(std::exception &e);
    virtual void setPKExceptionHandler(std::exception &e);
    virtual void setUniqueExceptionHandler(std::exception &e);
    virtual void setDefaultValueExceptionHandler(std::exception &e);
    virtual void addColumnExceptionHandler(std::exception &e);
    virtual void setForeignKeyExceptionHandler(std::exception &e);
    virtual void addTableExceptionHandler(std::exception &e);
    virtual void insertExceptionHandler(std::exception &e);
    virtual void selectFromExceptionHandler(std::exception &e);
    virtual void updateExceptionHandler(std::exception &e);
public:
    static Sqlite_wrapper *connectToDatabase(const std::string &fileName);

    void createTable(const std::string &table);
    void createColumn(const std::string &column, const std::string &type);
    void setAsPK();
    void setAsUnique();
    void setAsNotNullable();
    void setDefaultValue(const std::string &value);
    void addColumn();
    void setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn = "");
    void addTable();
    void dropTable(const std::string &table);//TODO
    void insertInto(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values);
    static void printToShell(std::shared_ptr<Query> result);
    std::shared_ptr<Query> selectFrom(const std::vector<std::string> &columns, const std::string table);
    std::shared_ptr<Query> selectFromOrderBy(const std::vector<std::string> &columns, const std::string table,
                                             const std::vector<std::string> &orderByColumn, const std::string &order = "asc");
    std::shared_ptr<Query> selectFromWhere(const std::vector<std::string> &columns, const std::string table,
                                           const std::vector<std::string> &where, const std::string &operand = ""/*and/or*/);
    std::shared_ptr<Query> selectFromLike(const std::vector<std::string> &columns, const std::string &table,
                                          const std::vector<std::string> &columnToCheck, const std::vector<std::string> &like,
                                          const std::string &operand = ""/*and/or*/);
    std::shared_ptr<Query> selectFromGlob(const std::vector<std::string> &columns, const std::string &table,
                                          const std::vector<std::string> &columnToCheck, const std::vector<std::string> &glob,
                                          const std::string &operand = ""/*and/or*/);
    std::string getID(const std::string &table, const std::string &columnName, const std::string &value, const std::string &IDName = "");
    //If IDName is not provided the IDName will be automatically set to table name with ID ending.
    //E.g. If table name is Test then IDName will be set to TestID
    void updateTable(const std::string &table, const std::vector<std::string> &columns,
                     const std::vector<std::string> &values, const std::string &where);
    void deleteRowFromTable(const std::string &table, const std::string &ID, const std::string &value);
    void clearTable(const std::string &table);
    void disconnectFromDatabase();

    virtual ~Sqlite_wrapper();
};

#endif // SQLITE_WRAPPER_H
