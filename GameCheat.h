#pragma once

#include <Windows.h>
#include <TlHelp32.h> 
#include <iostream>
using namespace std;

struct JX2Role
{
	int unknow[4]{};
	int hp[2]{};
	int tp[2]{ };
	int mp[2]{ };
	int shuxing[4]{};//攻击防御身法等级
	int unknow1[8]{};
	int x_y[2]{};
	int unknow2[3]{};
	int x_y1[4]{};
};
class typeData;

class GameCheat
{

public:
	GameCheat(DWORD Pid, DWORD  _Baseadress, DWORD _readtime = 100);//构造函数读取数据
	JX2Role data;                               //读取数据频率
	JX2Role& _data(bool IsCheckTime = true);
	void   SetData(LPVOID dataAdr, LPVOID buffer, SIZE_T lenth);
	
	typeData SetData(LPVOID dataAdr);
	~GameCheat()
	{
		if (hprocess)
			CloseHandle(hprocess);
	}
private:
	bool  ReadData(DWORD readTick);
	HANDLE hprocess;
	DWORD  Baseadress;
	DWORD  LastReadTime;
	DWORD  ReadTime;



};

class typeData
{
	GameCheat* pCheat;
	LPVOID dataAdr;
public:
	typeData(GameCheat* _pcheat, LPVOID _dataAdr) :pCheat{ _pcheat }, dataAdr{ _dataAdr } {}
	template<typename T>
	GameCheat& operator=(T val)
	{
		pCheat->SetData(dataAdr, &val, sizeof(T));
		return *pCheat;
	}


};