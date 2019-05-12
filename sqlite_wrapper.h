#ifndef SQLITE_WRAPPER_H
#define SQLITE_WRAPPER_H
#include <sqlite3.h>
#include <string>
#include <queue>
#include <map>
#include <vector>
#include <memory>

#include "exceptions.h"

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
        bool isForeignKey;
        std::queue<ForeignKey> foreignKeys;
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
    void _noResultExec(const std::string query);
    void _createDatabase(const std::string &fileName);
    void _createTable(const std::string &tableName);
    void _createColumn(const std::string &columnName, const std::string &type);
    void _setAsPK();
    void _setAsUnique();
    void _setAsNotNullable();
    void _setDefaultValue(const std::string &value);
    void _addColumn();
    void _setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn);
    void _addTable();
    void _dropTable(const std::string &tableName);//TODO
    void _insertInto(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values);
    void _selectFrom(const std::vector<std::string> &columns, const std::string table);
    void _selectFromWhere(const std::vector<std::string> &columns, const std::string table, const std::string &where);
    void _selectFromLike(const std::vector<std::string> &columns, const std::string &table,
                         const std::vector<std::string> &columnToCheck, const std::vector<std::string> &like,
                         const std::string & operand);
    void _updateTable(const std::string &tableName, const std::vector<std::string> &columns,
                      const std::vector<std::string> &values, const std::string &where);
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

    void createTable(const std::string &tableName);
    void createColumn(const std::string &columnName, const std::string &type);
    void setAsPK();
    void setAsUnique();
    void setAsNotNullable();
    void setDefaultValue(const std::string &value);
    void addColumn();
    void setForeinKey(const std::string &column, const std::string &refTable, const std::string &refColumn = "");
    void addTable();
    void dropTable(const std::string &tableName);//TODO
    void insertInto(const std::string &table, const std::vector<std::string> &columns, const std::vector<std::string> &values);
    static void printToShell(std::shared_ptr<Query> result);
    std::shared_ptr<Query> selectFrom(const std::vector<std::string> &columns, const std::string table);
    std::shared_ptr<Query> selectFromWhere(const std::vector<std::string> &columns, const std::string table, const std::string &where);
    std::shared_ptr<Query> selectFromLike(const std::vector<std::string> &columns, const std::string &table,
                                          const std::vector<std::string> &columnToCheck, const std::vector<std::string> &like,
                                          const std::string &operand = "or");
    void updateTable(const std::string &tableName, const std::vector<std::string> &columns,
                     const std::vector<std::string> &values, const std::string &where);
    void disconnectFromDatabase();

    virtual ~Sqlite_wrapper();
};

#endif // SQLITE_WRAPPER_H
