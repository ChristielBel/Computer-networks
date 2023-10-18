/*#include <stdafx.h>*/ // CLIENT TCP
#include <iostream>  
#define _WINSOCK_DEPRECATED_NO_WARNINGS  
// подавление предупреждений библиотеки winsock2
#include <winsock2.h> 
#include <string>
#include <windows.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable: 4996)  // подавление предупреждения 4996 
using namespace std;
#define SRV_HOST "localhost"  //адрес сервера
#define SRV_PORT 1234 //порт сервера
#define CLNT_PORT 1235  //порт клиента
#define BUF_SIZE 64  //размер сообщений
char TXT_ANSW[] = "I am your student\n";
int main() {
	char buff[1024]; //массив, котоорый содержит 1024 байта, память
	if (WSAStartup(0x0202, (WSADATA*)&buff[0])) {
		cout << "Error WSAStartup \n" << WSAGetLastError();  // Ошибка!
		return -1;
	}
	SOCKET s; //указатель на структуру сокета
	int from_len;   
	char buf[BUF_SIZE] = { 0 };    //для принятия для передачи реплик
	hostent* hp; //справочная структура, нужна для функции библеотеки сокета
	sockaddr_in clnt_sin, srv_sin; //инфа для сокета клиента, вторая для сервера
	s = socket(AF_INET, SOCK_STREAM, 0); //создание своего сокета домен версии ip версии подсоедтиняется tcp
	clnt_sin.sin_family = AF_INET; //домен версии ip версии
	clnt_sin.sin_addr.s_addr = 0; //0 означает адрес текущего компьютера
	clnt_sin.sin_port = htons(CLNT_PORT); //заполняем порт, функция htons() переводит константу в сетевой формат  
	bind(s, (sockaddr*)&clnt_sin, sizeof(clnt_sin)); //фнукция bind() в сокете всю инфу кладет в структуру сокета, пишем откуда взять инфу и ее размер
	hp = gethostbyname(SRV_HOST); //получаем числовой формат адреса
	srv_sin.sin_port = htons(SRV_PORT); //помещаем порт
	srv_sin.sin_family = AF_INET; //помещаем домен
	((unsigned long*)&srv_sin.sin_addr)[0] =
		((unsigned long**)hp->h_addr_list)[0][0]; //помещаем адрес 
	connect(s, (sockaddr*)&srv_sin, sizeof(srv_sin)); //связываемся с сервером(прокладывается виртуальный канал) КОННЕЕЕЕЕЕЕЕЕКТ
	string mst;
	//общаемся
	do {
		from_len = recv(s, (char*)&buf, BUF_SIZE, 0); //ждет информацию, функция возращает количество байтов, которые получила. В конец ставим 0, чтобы закончить
		buf[from_len] = 0;
		cout << buf << endl;
		//send (s, (char *)&TXT_ANSW, sizeof(TXT_ANSW),0); 
		getline(cin, mst); //считываем ответ клиента
		int msg_size = mst.size(); //узнаем размер
		send(s, (char*)&mst[0], msg_size, 0); // посылаем серверу, если не прощается, получаем ответ от сервера
	} while (mst != "Bye"); //пока клиент не попрощается
	cout << "exit to infinity" << endl; //уходим в бесконечность
	cin.get(); //задержка  
	closesocket(s); //разрываем соединение
	return 0;
}
