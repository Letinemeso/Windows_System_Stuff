#include "Mailbox_Manager.h"
#include "Process_Manager.h"
#include "Event.h"

#include <thread>
#include <chrono>

int main()
{
	Process_Manager process_manager;

	Mailbox_Manager mailbox_manager;

	Event event_from_server;
	event_from_server.create("event_from_server");
	if(!event_from_server.valid())
		return 1;

	mailbox_manager.create_mailbox("\\\\.\\mailslot\\CLIENT_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	process_manager.create_process("../Server/debug/Server.exe");
	if(process_manager.last_error())
		return 1;

	event_from_server.wait();

	Event event_from_client;
	event_from_client.create("event_from_client");
	if(!event_from_client.valid())
		return 1;

	mailbox_manager.connect_to_mailbox("\\\\.\\mailslot\\SERVER_MAILBOX");
	if(mailbox_manager.last_error())
		return 1;

	event_from_client.activate();

//	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	event_from_server.wait();

	std::cout << "Client: messages should be sent\n";

	std::vector<std::string> messages = mailbox_manager.get_messages();
	if(mailbox_manager.last_error())
		return 1;

	for(unsigned int i=0; i<messages.size(); ++i)
		std::cout << messages[i] << "\n";

	std::cout << "Client: messages received\n";

	event_from_client.activate();

	process_manager.wait_for_process_end();

	event_from_client.abandon();
	event_from_server.abandon();

	return 0;
}


/*
// Программа из конспекта "Системное программное обеспечение"
// Файлы, отображаемые на память, синхронизация событиями
// стр. ??
// Приложение MFE_CLIENT (клиентское приложение)
// Демонстрация использования файлов, отображаемых на память,
// для передачи данных между процессами, синхронизированными событиями (Events)

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

// Идентификаторы объектов-событий, которые используются
// для синхронизации задач, принадлежащих разным процессам
HANDLE hEventSend;
HANDLE hEventRecv;
HANDLE hEventTermination;
// Имя объектов-событий для синхронизации записи и чтения из отображаемого файла
CHAR lpEventSendName[] =
"$MylEventSendName$";
CHAR lpEventRecvName[] =
"$MylEventRecvName$";
// Имя объекта-события для завершения процесса
CHAR lpEventTerminationName[] =
"$MyEventTerminationName$";
// Имя отображния файла на память
CHAR lpFileShareName[] =
"$MyFileShareName$";
// Идентификатор отображения файла на память
HANDLE hFileMapping;
// Указатель на отображенную область памяти
LPVOID lpFileMap;

int main()
{
	CHAR str[80];
	DWORD dwRetCode;
	printf("Mapped and shared file, event_from_server sync, client process\n"
		"\n\nEnter  <Exit> to terminate...\n");
	// Открываем объекты-события для синхронизации
	// чтения и записи
	hEventSend = OpenEvent(EVENT_ALL_ACCESS, FALSE, (wchar_t*)lpEventSendName);
	hEventRecv = OpenEvent(EVENT_ALL_ACCESS, FALSE, (wchar_t*)lpEventRecvName);
	if (hEventSend == NULL || hEventRecv == NULL)
	{
		fprintf(stdout, "OpenEvent: Error %ld\n",
			GetLastError());
		getch();
		return 0;
	}
	// Открываем объект-событие для сигнализации о
	// завершении процесса
	hEventTermination = OpenEvent(EVENT_ALL_ACCESS,
		FALSE, (wchar_t*)lpEventTerminationName);
	if (hEventTermination == NULL)
	{
		fprintf(stdout, "OpenEvent (Termination): Error %ld\n",
			GetLastError());
		getch();
		return 0;
	}
	// Открываем объект-отображение
	hFileMapping = OpenFileMapping(
		FILE_MAP_READ | FILE_MAP_WRITE, FALSE, (wchar_t*)lpFileShareName);
	// Если открыть не удалось, выводим код ошибки
	if (hFileMapping == NULL)
	{
		fprintf(stdout, "OpenFileMapping: Error %ld\n",
			GetLastError());
		getch();
		return 0;
	}
	// Выполняем отображение файла на память.
	// В переменную lpFileMap будет записан указатель на
	// отображаемую область памяти
	lpFileMap = MapViewOfFile(hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	// Если выполнить отображение не удалось,
	// выводим код ошибки
	if (lpFileMap == 0)
	{
		fprintf(stdout, "MapViewOfFile: Error %ld\n",
			GetLastError());
		getch();
		return 0;
	}

	// Цикл чтения/записи данных. Этот цикл завершает свою работу,
	// когда пользователь набирает коанду <Exit>,
	while (TRUE)
	{
		// Читаем введенную строку
		gets(str);
		// Записываем строку в отображенную память,
		// доступную серверному процессу
		strcpy((char*)lpFileMap, str);
		// Если введена команда <Exit>, прерываем цикл
		if (!strcmp(str, "exit") || !strcmp(str, "Exit") || !strcmp(str, "EXIT"))
			break;
		// Записываем строку в отображенную память,
		// доступную серверному процессу
		strcpy((char*)lpFileMap, str);
		// Устанавливаем объект-событие в отмеченное
		// состояние
		SetEvent(hEventSend);
		// ждем ответа
		dwRetCode = WaitForSingleObject(hEventRecv, INFINITE);
		// если ответ получен - выводим, если ошибка - выходим
		if (dwRetCode == WAIT_OBJECT_0) puts(((LPSTR)lpFileMap));
		if (dwRetCode == WAIT_ABANDONED_0 || dwRetCode == WAIT_FAILED)
		{
			printf("\nError waiting responce!\n)");
			//break;
		}
	}
	// После завершения цикла переключаем все события
	// в отмеченное состояние для отмены ожидания в
	// процессе отображения и для завершения этого процесса
	//SetEvent(hEventSend);
	SetEvent(hEventTermination);
	// Закрываем идентификаторы объектов-событий
	CloseHandle(hEventSend);
	CloseHandle(hEventRecv);
	CloseHandle(hEventTermination);
	// Отменяем отображение файла
	UnmapViewOfFile(lpFileMap);
	// Освобождаем идентификатор созданного
	// объекта-отображения
	CloseHandle(hFileMapping);

	return 0;
}*/
