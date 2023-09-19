#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
using namespace std;

struct Person
{
	string filename;
	string name;  //имя
	int grades[4];  //оценки
}B;
long size_pred;
int main() {
	ifstream fR;
	ofstream fA;
	setlocale(LC_ALL, "rus");
	string nameR = "D:/request.bin"; //файл запросов клиентов
	string nameA;

	fA.open(nameR);
	fA.close();
	cout << "server is working" << endl;
	// начальные установки
	fR.open(nameR, ios::binary); //открытие файла REQUEST
	fR.seekg(0, ios::end);
	size_pred = fR.tellg(); //стартовая позиция сервера в файле REQUEST
	fR.close();

	// начало работы сервера
	while (true)
	{
		fR.open(nameR, ios::binary); //открытие файла REQUEST
		fR.seekg(0, ios::end); //переход в конец файла REQUEST

		// есть новые запросы от клиентов? 
		while (size_pred >= fR.tellg())
		{
			fR.close();
			Sleep(100);
			fR.open(nameR, ios::binary);
			fR.seekg(0, ios::end);
		}
		int answer = -1;
		// получен новый запрос 
		fR.seekg(size_pred, ios::beg); //переход к началу полученного запроса
		fR.read((char*)&B, sizeof(B)); //считывание данных клиента
		size_pred = fR.tellg(); // на конец обработанных данных
		fR.close();

		cout << B.filename << " " << B.name << " ";
		for (int i = 0; i < 4; i++) {
			cout << B.grades[i] << " ";
		}
		// определение индекса массы 
		cout << answer << endl;
		if (B.grades[0] == 2 || B.grades[1] == 2 || B.grades[2] == 2 || B.grades[3] == 2) answer = 2;
		else if (B.grades[0] == 3 || B.grades[1] == 3 || B.grades[2] == 3|| B.grades[3] == 3) answer = 3;
		else if (B.grades[0] == 5 && B.grades[1] == 5 && B.grades[2] == 5 && B.grades[3] == 5) answer = 5;
		else answer = 4;
		cout << answer << endl;
		nameA = "D:/" + B.filename + ".bin";

		// передача ответа клиенту
		fA.open(nameA, ios::binary | ios::app); //открытие файла ANSWER
		fA.write((char*)&answer, sizeof(answer)); //запись ответа клиенту
		fA.close();
	}// while
}
