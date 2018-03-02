#pragma once
#include "stdafx.h"
#include "Common.h"

class Task
{
public:
	static int ProcessRecvData(WPARAM wParam, LPARAM lParam);
	static int GetFriends(WPARAM wParam, LPARAM lParam);

	static void Callback(int result);


};

