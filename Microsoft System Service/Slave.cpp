#include <iostream>
#include "Slave.h"
#include "Protocol.h"
#include "UpdateThread.h"
#include "OS.h"

void Slave::Run() {
    if (m_Socket.bind(Port) != sf::Socket::Done)throw "Can't bind to socket";

    m_Socket.setBlocking(true);

    UpdateThread::New();

    for (;;) {
        sf::Packet packet;
        sf::IpAddress source_ip;
        sf::Uint16 source_port;

        m_Socket.receive(packet, source_ip, source_port);

        std::cout << "Slave: Got Datagram\n";

        Datagram dg(DatagramType::Request);

        packet >> dg;

        ProcessDatagram(dg, source_ip, source_port);
    }
}

void Slave::Close() {
    m_Socket.unbind();
}

void Slave::ProcessDatagram(const Datagram &dg, sf::IpAddress source_ip, sf::Uint16 source_port) {
    if (dg.MagicWord == MagicWord) {
        switch (dg.Type) {
        case DatagramType::Signal:
            return ProcessSignal(dg.SignalData, source_ip, source_port);
        case DatagramType::Request:
            return ProcessRequest(dg.RequestData, source_ip, source_port);
        default:
            std::cout << "Datagram: unknown type\n";
            return;
        }
    } 
    std::cout << "Datagram: Magic Word does not match\n";
}

void Slave::ProcessSignal(const Signal& signal, sf::IpAddress source_ip, sf::Uint16 source_port) {
    Datagram dg(DatagramType::Reply);
    dg.ReplyData.Type = ReplyType::SignalSuccess;

    Send(dg, source_ip, source_port);

    switch (signal.Type) {
    case SignalType::Shutdown:
        return OS::Shutdown();
    case SignalType::Message:
        return OS::CreateMessageBox("Error",signal.SignalMessageData.Message);
    default:
        std::cout << "Signal: Unknown type\n";
    }
}

void Slave::ProcessRequest(const Request& request, sf::IpAddress source_ip, sf::Uint16 source_port) {
    switch (request.Type) {
    case RequestType::SystemInfo:
        return SystemInfoReply(source_ip, source_port);
    default:
        std::cout << "Request: Unknown type\n";
    }
}

void Slave::SystemInfoReply(sf::IpAddress source_ip, sf::Uint16 source_port) {
    Datagram dg(DatagramType::Reply);
    dg.ReplyData.Type = ReplyType::SystemInfo;
    dg.ReplyData.SystemInfoData.BootupTime = OS::GetBootupTime();
    dg.ReplyData.SystemInfoData.NameLength = sizeof(dg.ReplyData.SystemInfoData.PCName) / sizeof(char);
    OS::GetPCName(dg.ReplyData.SystemInfoData.PCName, &dg.ReplyData.SystemInfoData.NameLength);

    Send(dg, source_ip, source_port);
}

void Slave::Send(const Datagram& dg, sf::IpAddress ip, sf::Uint16 port) {
    sf::Packet packet;
    packet << dg;
    m_Socket.send(packet, ip, port);
}

