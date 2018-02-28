#include "DBModule.h"
#include "Common.h"


DBModule::DBModule()
{
	InitializeCriticalSection(&csLock);
}


DBModule::~DBModule()
{
	DeleteCriticalSection(&csLock);
}

BOOL DBModule::Init()
{
	if (sqliteQuery.Open(GetInstancePath() + L"ChatServer.db") == FALSE)
		return FALSE;
	if (sqliteModify.Open(GetInstancePath() + L"ChatServer.db") == FALSE)
	{
		sqliteQuery.Close();
		return FALSE;
	}
	return TRUE;
}

BOOL DBModule::SignIn(SignInParam * param)
{
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select Password from User where Account='%s'", param->account);
	SQLiteDataReader reader = sqliteQuery.ExcuteQuery(sql);
	if (reader.Read() == NULL)
		return FALSE;
	else
	{
		LPCTSTR password = reader.GetStringValue(0);
		if (_wcsicmp(password, param->password) == 0)
			return TRUE;
		else
			return FALSE;
	}
}

BOOL DBModule::SignUp(User *user)
{
	BOOL bSuccess = FALSE;
	EnterCriticalSection(&csLock);
	WCHAR sql[1024] = {};
	StringCbPrintf(sql, 1024, L"select * from User where Account='%s'", user->account);
	SQLiteDataReader reader = sqliteQuery.ExcuteQuery(sql);
	if (reader.Read() == NULL)
	{
		StringCbPrintf(sql, 1024, L"insert into User (Account,Password,HeaderImg,NickName,Sexulity,Signature,Area,Phone) VALUES ('%s','%s',%d, '%s',%d,'%s','%s','%s')", user->account, user->password, user->headerImg, user->nickName, user->sexulity, user->signature, user->area, user->phone);
		if (sqliteModify.ExcuteModify(sql))
		{
			bSuccess = TRUE;
		}
	}
	LeaveCriticalSection(&csLock);
	return bSuccess;
}
