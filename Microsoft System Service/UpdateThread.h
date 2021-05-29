#pragma once

#include <SFML/Network.hpp>

class UpdateThread {
private:
	sf::TcpListener m_Listener;
	sf::TcpSocket m_Socket;
public:

	void Run();

	static void New();
private:
	void RecieveUpdate();

	void ProcessUpdate(const void *data, size_t size);
};
