#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
using namespace std;
// структура данных запроса клиента
struct Person
{
	string filename;
	string name;  //имя
	int grades[4];  //оценки
} A;
long pred_size;
void main() {
	setlocale(LC_ALL, "rus");
	string nameR = "D:/request.bin"; //файл для запросов клиентов
	ofstream f_REQ;
	ifstream f_ANS;

	while (true)
	{
		// передача данных от клиента серверу
		cout << "Введите запрос: Название клиента Фамилия Оценки(4 штуки)" << endl;
		cin >> A.filename >> A.name;
		for (int i = 0; i < 4; i++)
			cin >> A.grades[i];

		string nameA = "D:/" + A.filename + ".bin";

		f_REQ.open(nameR, ios::app | ios::binary); //открытие файла REQUEST 
		f_REQ.write((char*)&A, sizeof(A)); //запись запроса в файл REQUEST
		f_REQ.close();

		f_REQ.open(nameA);
		f_REQ.close();
		int answer = -1;
		// поступил ответ от сервера?
		f_ANS.open(nameA, ios::binary); //открытие файла ANSWER
		f_ANS.seekg(0, ios::end); //переход в конец файла ANSWER
		pred_size = f_ANS.tellg();
		while (pred_size >= f_ANS.tellg())
		{
			Sleep(100); // ждем и переходим в конец файла ANSWER
			f_ANS.seekg(0, ios::end);
		}
		cout << answer << endl;
		// получение ответа от сервера 
		f_ANS.seekg(pred_size, ios::beg); // на начало нового ответа
		f_ANS.read((char*)&answer, sizeof(answer)); //считывание ответа
		f_ANS.close();
		cout << answer << endl;
		// расшифровка ответа
		switch (answer) {
		case 2: {cout << "Задолженность.Стипендия не выплачивается\n"; break; }
		case 3: {cout << "Стипендия не выплачивается\n"; break; }
		case 4: {cout << "1000 \t Обычная стипендия\n"; break; }
		case 5: {cout << "2000 \t Повышенная стипендия\n"; break; }
		case -1: {cout << "Error, server sent wrong data"; }
		}
	} //while
}
