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
struct Person
{
	string name;  //имя
	int grades[4];  //оценки
}B;
const string QUEST = "Enter the data\n"; //первый вопрос для клиента, чтобы начать диалог
int answer;
int main() {
	setlocale(LC_ALL, "rus");
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
	listen(s, 6); //создает очередь в 6 клиентов
	//сервер работает бесконечно долго
	while (1) {
		from_len = sizeof(from_sin);
		s_new = accept(s, (sockaddr*)&from_sin, &from_len); //из очереди, в которой могут стоть 3 клиента, вынимает клиента, в структуру from_sin 
		//попадает итнформация о клиенте, а вторая функция указывает размер
		cout << "new connected client! " << endl; //говорим, что подключился клиент
		msg = QUEST;
		send(s_new, (char*)&msg[0], msg.size(), 0);
		recv(s_new, (char*)&B, sizeof(B), 0);
		if (B.grades[0] == 2 || B.grades[1] == 2 || B.grades[2] == 2 || B.grades[3] == 2) answer = 2;
		else if (B.grades[0] == 3 || B.grades[1] == 3 || B.grades[2] == 3 || B.grades[3] == 3) answer = 3;
		else if (B.grades[0] == 5 && B.grades[1] == 5 && B.grades[2] == 5 && B.grades[3] == 5) answer = 5;
		else answer = 4;
		send(s_new, (char*)&answer, sizeof(answer), 0);
		while (1) {
			//int bytesSent = send(s_new, (char*)&msg[0], msg.size(), 0); //посылаем сообщение
			//if (bytesSent == SOCKET_ERROR) {
			//	//cout << "Error sending data to client: " << WSAGetLastError() << endl;
			//	break;
			//} //проверяет ошибки при отправке данных клиенту
			int bytesReceived = recv(s_new, (char*)buf, BUF_SIZE, 0); //получаем новое сообщение
			if (bytesReceived == SOCKET_ERROR) {
				//cout << "Error receiving data from client: " << WSAGetLastError() << endl;
				break;
			} //проверка ошибок при получении сообщения от клиента
			if (bytesReceived == 0) {
				cout << "client is lost" << endl;
				break;
			}
			buf[bytesReceived] = 0;
			msg1 = (string)buf;
			cout << "client: ";
			cout << msg1 << endl;
			if (msg1 == "Bye") break;//если бай, то разрываем диалог, но если не бай, то общаемся дальше
			cout << "server: ";
			getline(cin, msg);
			int bytesSent = send(s_new, (char*)&msg[0], msg.size(), 0); //посылаем сообщение
			if (bytesSent == SOCKET_ERROR) {
				//cout << "Error sending data to client: " << WSAGetLastError() << endl;
				break;
			}
		}
		cout << "client is lost" << endl;
		closesocket(s_new); //разрываем связь с клиентом и уходим общаться с другим клиентом
	}
	return 0;
}
