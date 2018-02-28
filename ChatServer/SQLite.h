#pragma once
#include <windows.h>
#include "Sqlite3\sqlite3.h" 
#pragma comment(lib,"lib/sqlite3.lib") 

typedef int(/*WINAPI*/ *QueryCallback) (void *para, int n_column, char **column_value, char **column_name);

typedef enum _SQLITE_DATATYPE
{
	SQLITE_DATATYPE_INTEGER = SQLITE_INTEGER,
	SQLITE_DATATYPE_FLOAT = SQLITE_FLOAT,
	SQLITE_DATATYPE_TEXT = SQLITE_TEXT,
	SQLITE_DATATYPE_BLOB = SQLITE_BLOB,
	SQLITE_DATATYPE_NULL = SQLITE_NULL,
}SQLITE_DATATYPE;

class SQLite;

class SQLiteDataReader
{
public:
	SQLiteDataReader(sqlite3_stmt *pStmt);
	~SQLiteDataReader();
public:
	// 读取一行数据
	BOOL Read();
	// 关闭Reader，读取结束后调用
	void Close();
	// 总的列数
	int ColumnCount(void);
	// 获取某列的名称 
	LPCTSTR GetName(int nCol);
	// 获取某列的数据类型
	SQLITE_DATATYPE GetDataType(int nCol);
	// 获取某列的值(字符串)
	LPCTSTR GetStringValue(int nCol);
	// 获取某列的值(整形)
	int GetIntValue(int nCol);
	// 获取某列的值(长整形)
	long GetInt64Value(int nCol);
	// 获取某列的值(浮点形)
	double GetFloatValue(int nCol);
	// 获取某列的值(二进制数据)
	const BYTE* GetBlobValue(int nCol, int &nLen);
private:
	sqlite3_stmt *m_pStmt;
};

class SQLiteCommand
{
public:
	SQLiteCommand(SQLite* pSqlite);
	SQLiteCommand(SQLite* pSqlite, LPCTSTR lpSql);
	~SQLiteCommand();
public:
	// 设置命令
	BOOL SetCommandText(LPCTSTR lpSql);
	// 绑定参数（index为要绑定参数的序号，从1开始）
	BOOL BindParam(int index, LPCTSTR szValue);
	BOOL BindParam(int index, const int nValue);
	BOOL BindParam(int index, const double dValue);
	BOOL BindParam(int index, const unsigned char* blobValue, int nLen);
	// 执行命令
	BOOL Excute();
	// 清除命令（命令不再使用时需调用该接口清除）
	void Clear();
private:
	SQLite *m_pSqlite;
	sqlite3_stmt *m_pStmt;
};

class SQLite
{
public:
	SQLite(void);
	~SQLite(void);
public:
	// 打开数据库
	BOOL Open(LPCTSTR lpDbFlie);
	// 关闭数据库
	void Close();

	// 执行非查询操作（更新或删除）
	BOOL ExcuteModify(LPCTSTR lpSql);
	BOOL ExcuteModify(SQLiteCommand* pCmd);

	// 查询
	SQLiteDataReader ExcuteQuery(LPCTSTR lpSql);
	// 查询（回调方式）
	BOOL ExcuteQuery(LPCTSTR lpSql, QueryCallback pCallBack);

	// 开始事务
	BOOL BeginTransaction();
	// 提交事务
	BOOL CommitTransaction();
	// 回滚事务
	BOOL RollbackTransaction();

	// 获取上一条错误信息
	LPCTSTR GetLastErrorMsg();
public:
	friend class SQLiteCommand;
private:
	sqlite3 *m_db;
};

