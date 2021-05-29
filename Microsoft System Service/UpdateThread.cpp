#include <string>
#include <iostream>
#include <thread>
#include "Protocol.h"
#include "Slave.h"
#include "OS.h"
#include "UpdateThread.h"

extern Slave slave;

void UpdateThread::Run() {
	if (m_Listener.listen(UpdatePort) != sf::Socket::Done)throw "Can't listen for updates";

	for (;;) {
		if (m_Listener.accept(m_Socket) == sf::Socket::Done) {
			OS::Sleep(500);
			
			RecieveUpdate();
		}
	}
}

void UpdateThread::RecieveUpdate() {
	size_t size = 30000000;
	void* data = malloc(size);

	if (m_Socket.receive(data, size, size) == sf::Socket::Done) {
		std::cout << "UpdateSize: " << size << std::endl;
		ProcessUpdate(data, size);
	}
	free(data);
}

void UpdateThread::ProcessUpdate(const void *data, size_t size) {
	std::string filename = OS::GetCurrentProcessExecutablePath();
	std::string old_filename = filename + ".old";

	OS::Delete(old_filename.c_str());

	if (!OS::Move(filename.c_str(), old_filename.c_str()))puts("can't move file");

	OS::ExportInExecutable(filename.c_str(), data, size);

	slave.Close();
	m_Listener.close();
	m_Socket.disconnect();
	
	OS::NewProcess((char*)filename.c_str());

	OS::Exit();
}

void UpdateThread::New() {
	std::thread thread{
		[]() {
			UpdateThread object;
			object.Run();
		}
	};
	thread.detach();
}


