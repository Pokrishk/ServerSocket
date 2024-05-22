#define WIN32_LEAN_AND_MEAN //макрос

#include <iostream> //нужные библиотеки
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
	WSADATA wsaData; // служебная структура для хранение информации
	ADDRINFO hints; //структура для указания критериев для функции getaddrinfo.
	ADDRINFO* adrrResult; //указатель на структуру с результатами вызова getaddrinfo.
	SOCKET ConnectSocket = INVALID_SOCKET; //сокет
	const char* sendBuffer = "Hello from Server"; //буфер с отправляемым сообщением
	char recvBuffer[512]; //буфер с получаемым сообщением
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //старт использования библиотеки сокетов процессом определния версии и структуры
	SOCKET ServerSocket = INVALID_SOCKET; //сокет, который слушает соединение
	if (result != 0) { //обработка ошибки
		cout << "WSAStartu failed error" << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));//нам необходимо изначально занулить память, 1-ый паметр, что зануляем,2-ой сколько
	hints.ai_family = AF_INET;//4-байтный Ethernet
	hints.ai_socktype = SOCK_STREAM; //задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;//используем протокол TCP
	hints.ai_flags = AI_PASSIVE;//Пассивная сторона, потому что просто ждет соединения

	result = getaddrinfo(NULL, "3030", &hints, &adrrResult); //функциия хранит в себе адрес, порт,семейство структур, адрес сокета
	if (result != 0) { //обработка ошибки с отчисткой памяти
		cout << "getaddrinfo failed" << endl;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}

	ServerSocket = socket(adrrResult->ai_family, adrrResult->ai_socktype, adrrResult->ai_protocol); //создание сокета с указателями на его параметры
	if (ServerSocket == INVALID_SOCKET) { //обработка ошибки с отчисткой памяти
		cout << "Socket creation with" << endl;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}

	result = bind(ServerSocket, adrrResult->ai_addr, (int)adrrResult->ai_addrlen); //привязываем сокет к IP-адресу (соединились с сервером)
	if (result == SOCKET_ERROR) { //обработка ошибки с отчисткой памяти
		cout << "Unable connect to server" << endl;
		closesocket(ServerSocket);
		ServerSocket = INVALID_SOCKET;
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}
	result = listen(ServerSocket, SOMAXCONN); //перевод сокета в режим прослушивания для ожидания входящих соединений.
	if (result == SOCKET_ERROR) { //обработка ошибки с отчисткой памяти
		cout << "Listening socket failed" << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}

	ConnectSocket = accept(ServerSocket, NULL, NULL); //ожидание и принятие входящего соединения.
	if (ConnectSocket == SOCKET_ERROR) { //обработка ошибки с отчисткой памяти
		cout << "Accepting socket failed" << endl;
		closesocket(ConnectSocket);
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}

	do {
		ZeroMemory(recvBuffer, 512);//обнуление буфера для получения данных.
		result = recv(ConnectSocket, recvBuffer, 512, 0); //получение данных от клиента. (сокет, буфер для получения, размер, флаги)
		if (result > 0) { //вывод полученного сообщения и его размера в консоль
			cout << "Received " << result << "bytes" << endl;
			cout << "Received data " << recvBuffer << endl;


			result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);//отправка данных клиенту (сокет, что нужно отправить, размер отправимого, флаги (0-знач. по умолчанию))

			if (result == SOCKET_ERROR) { //обработка ошибки отправки с отчисткой памяти
				cout << "Failed to send data back" << endl;
				closesocket(ConnectSocket);
				freeaddrinfo(adrrResult);
				WSACleanup();
			}
		}
		else if (result == 0)
			cout << "Connection closing..." << endl; //сообщение о закрытии соединения

		else
		{
			cout << "recv failed with error" << endl; //обработка ошибки принятия данных с отчисткой памяти
			closesocket(ConnectSocket);
			freeaddrinfo(adrrResult);
			WSACleanup();
			return 1;
		}


	} while (result > 0); //продолжение цикла, пока есть получаемые данные.

	result = shutdown(ConnectSocket, SD_SEND); //остановка передачи данных на сокете
	if (result == SOCKET_ERROR) { //обработка ошибки с отчисткой памяти
		closesocket(ConnectSocket);
		freeaddrinfo(adrrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket); //очистка памяти
	freeaddrinfo(adrrResult);
	WSACleanup();
	return 0;
}

