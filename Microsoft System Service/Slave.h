#pragma once

#include <SFML/Network.hpp>
#include "Protocol.h"

class Slave {
private:
    sf::UdpSocket m_Socket;
public:
    void Run();

    void Close();
private:
    void ProcessDatagram(const Datagram& dg, sf::IpAddress source_ip, sf::Uint16 source_port);

    void ProcessSignal(const Signal& signal, sf::IpAddress source_ip, sf::Uint16 source_port);

    void ProcessRequest(const Request& request, sf::IpAddress source_ip, sf::Uint16 source_port);

    void SystemInfoReply(sf::IpAddress source_ip, sf::Uint16 source_port);

    void Send(const Datagram& dg, sf::IpAddress ip, sf::Uint16 port);
};

