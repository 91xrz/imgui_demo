#include "GameCheat.h"



bool GameCheat::ReadData(DWORD readTick)
{
	if (!hprocess)	return false;
	LastReadTime = readTick;
	return ReadProcessMemory(hprocess, (LPVOID)Baseadress, this, sizeof(JX2Role), NULL);//��ȡ���ݽ������Ա

}

GameCheat::GameCheat(DWORD Pid, DWORD _Baseadress, DWORD _readtime)
{
	ReadTime = _readtime;
	Baseadress = _Baseadress;
	hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);//��ȡ���̾��
	//return (hprocess != NULL);
}

JX2Role& GameCheat::_data(bool IsCheckTime)  // read������ȡ�����ݽ���data  ����data������ʡ�ڴ� 
{
	if (!IsCheckTime) return data;
	DWORD dTickNow = GetTickCount64();

	if ((dTickNow - LastReadTime) > ReadTime) ReadData(dTickNow);
	return data;

}

void GameCheat::SetData(LPVOID dataAdr, LPVOID buffer, SIZE_T lenth)
{
	LPVOID destAdr = (LPVOID)((unsigned)dataAdr - (unsigned)this + Baseadress);//����Ŀ��ƫ����
	WriteProcessMemory(hprocess, destAdr, buffer, lenth, 0);

}



typeData GameCheat::SetData(LPVOID dataAdr)
{
	return typeData(this, dataAdr);
}
