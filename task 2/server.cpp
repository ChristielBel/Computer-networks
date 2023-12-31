// SERVER TCP 10.16.4.132 IP
//#include <stdafx.h> 
#include <iostream>  
#include <winsock2.h> 
#include <windows.h> 
#include <string> 
#pragma comment (lib, "Ws2_32.lib")  
using namespace std;
#define SRV_PORT 1234  //порт сервера(его обязательно должен знать клиент)
#define BUF_SIZE 64  //размер
const string QUEST = "Who are you?\n"; //первый вопрос для клиента, чтобы начать диалог
int main() {
	char buff[1024];
	if (WSAStartup(0x0202, (WSADATA*)&buff[0]))
	{
		cout << "Error WSAStartup \n" << WSAGetLastError();   // Ошибка!
		return -1;
	}
	SOCKET s, s_new; //2 сокета, s - сервера, s_new -клиента, коитоорый подключился
	int from_len;
	char buf[BUF_SIZE] = { 0 };
	sockaddr_in sin, from_sin; //2 струкртуры, s - сервера, s_new -клиента, котоорый подключился
	s = socket(AF_INET, SOCK_STREAM, 0); //создание сокета, указывается адрес, должго быть одно и тоже соединение
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0; //адрес текущего компа
	sin.sin_port = htons(SRV_PORT);
	bind(s, (sockaddr*)&sin, sizeof(sin)); //добавляем информацию в сокет
	string msg, msg1;
	listen(s, 3); //создает очередь в три клиента
	//сервер работает бесконечно долго
	while (1) {
		from_len = sizeof(from_sin);
		s_new = accept(s, (sockaddr*)&from_sin, &from_len); //из очереди, в которой могут стоть 3 клиента, вынимает клиента, в структуру from_sin 
		//попадает итнформация о клиенте, а вторая функция указывает размер
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(s_new, &fd);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		cout << "new connected client! " << endl; //говорим, что подключился клиент
		msg = QUEST;
		while (1) {
			if(WSAGetLastError() == WSAENOTCONN || WSAGetLastError() == WSAECONNRESET || send(s_new, (char*)&msg[0], msg.size(), 0)==SOCKET_ERROR) break; //посылаем сообщение
			from_len = recv(s_new, (char*)buf, BUF_SIZE, 0); //получаем новое сообщение
			buf[from_len] = 0;
			msg1 = (string)buf;
			cout << "client: ";
			cout << msg1 << endl;
			//if (!select(s_new + 1, NULL, &fd, NULL, &tv)) { cout << "aaaa";  break; }
			//cout << select(s_new + 1, NULL, &fd, NULL, &tv);
			if (msg1 == "q") break;//если бай, то разрываем диалог, но если не бай, то общаемся дальше
			cout << "server: ";
			getline(cin, msg);
		}
		cout << "client is lost" << endl;
		closesocket(s_new); //разрываем связь с клиентом и уходим общаться с другим клиентом
	}
	return 0;
}
