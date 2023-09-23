#include "GameCheat.h"



bool GameCheat::ReadData(DWORD readTick)
{
	if (!hprocess)	return false;
	LastReadTime = readTick;
	return ReadProcessMemory(hprocess, (LPVOID)Baseadress, this, sizeof(JX2Role), NULL);//读取数据进入类成员

}

GameCheat::GameCheat(DWORD Pid, DWORD _Baseadress, DWORD _readtime)
{
	ReadTime = _readtime;
	Baseadress = _Baseadress;
	hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);//获取进程句柄
	//return (hprocess != NULL);
}

JX2Role& GameCheat::_data(bool IsCheckTime)  // read函数读取到数据进入data  返回data的引用省内存 
{
	if (!IsCheckTime) return data;
	DWORD dTickNow = GetTickCount64();

	if ((dTickNow - LastReadTime) > ReadTime) ReadData(dTickNow);
	return data;

}

void GameCheat::SetData(LPVOID dataAdr, LPVOID buffer, SIZE_T lenth)
{
	LPVOID destAdr = (LPVOID)((unsigned)dataAdr - (unsigned)this + Baseadress);//计算目标偏移量
	WriteProcessMemory(hprocess, destAdr, buffer, lenth, 0);

}



typeData GameCheat::SetData(LPVOID dataAdr)
{
	return typeData(this, dataAdr);
}
