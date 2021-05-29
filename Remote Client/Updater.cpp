#include <iostream>
#include <SFML/Network.hpp>
#include "Protocol.h"

const char* address = "192.168.0.93";
const char* update_file = R"(S:\SharedDev\Microsoft System Service\Release\Microsoft System Service.exe)";

int main(){
	sf::TcpSocket socket;
	if (socket.connect(sf::IpAddress(address), UpdatePort, sf::seconds(0.5f)) == sf::Socket::Done) {
		FILE* file;
		fopen_s(&file, update_file, "rb");
		if (file) {
			fseek(file, 0, SEEK_END);
			size_t size = ftell(file);
			fseek(file, 0, SEEK_SET);

			void* data = malloc(size);
			fread(data, 1, size, file);

			fclose(file);

			std::cout << "Update size: " << size << std::endl;

			if (socket.send(data, size) == sf::Socket::Done)puts("Success");
		} else {
			std::cout << "Failed to open file\n";
		}
		socket.disconnect();
	} else {
		std::cout << "Failed to update\n";
	}
	getchar();
}