#include "stdafx.h"
#include "Winsock2.h"
#include <iostream>
#include <string>
#include <ctime>
#include "Error.h"
#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable : 4996)
using namespace std;

struct GETSINCHRO{string cmd; int curvalue;};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, "Russian");
	string IP = "172.20.10.11";
	//string IP = "127.0.0.1";
	int Tc = 1000; 
	SYSTEMTIME tm;
	GETSINCHRO getsincro, setsincro;
	ZeroMemory(&setsincro, sizeof(setsincro));
	ZeroMemory(&getsincro, sizeof(getsincro));
	getsincro.cmd = "SINC";
	getsincro.curvalue = 0;

	cout << "Client: " << endl;
	try
	{
		SOCKET cS;
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		if ((cS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("Socket: ", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(IP.c_str());
		int maxcor = 0;
		int mincor = INT_MAX;
		int avgcorr = 0;
		int lensockaddr = sizeof(serv);

		for (int i = 0; i < 11; i++)
		{
			GetSystemTime(&tm);
			sendto(cS, (char*)&getsincro, sizeof(getsincro), 0, (sockaddr*)&serv, sizeof(serv));
			recvfrom(cS, (char*)&setsincro, sizeof(setsincro), 0, (sockaddr*)&serv, &lensockaddr);

			maxcor = (maxcor < setsincro.curvalue) ? setsincro.curvalue : maxcor;
			mincor = (mincor > setsincro.curvalue) ? setsincro.curvalue : mincor;
			avgcorr += setsincro.curvalue;

			cout << tm.wDay << "." << tm.wMonth << "." << tm.wYear << " - " << tm.wHour + 3 << ":" << tm.wMinute << ":" << tm.wSecond << ":"
				<< tm.wMilliseconds << " " << endl << i + 1 << " " << getsincro.curvalue << " correction = " << setsincro.curvalue << " max correction/min correction: " << maxcor << "/" << mincor << endl << endl << endl;

			if (i == 0)	getsincro.curvalue += setsincro.curvalue;
			else getsincro.curvalue += setsincro.curvalue + Tc;

			Sleep(Tc);
		}
		cout << "Average correction: " << avgcorr / 10 << endl;

		if (closesocket(cS) == SOCKET_ERROR)
			throw SetErrorMsgText("Closesocket: ", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText){cout << endl << errorMsgText << endl;}
	return 0;
}