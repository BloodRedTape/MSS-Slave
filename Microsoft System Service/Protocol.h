#pragma once

#include <SFML/Network.hpp>

constexpr sf::Uint16 Port = 5432;
constexpr sf::Uint16 UpdatePort = 5445;
constexpr sf::Uint64 MagicWord = 0xBBAADD0000DDAABB;

enum class DatagramType: sf::Uint8{
	Request = 0x01,
	Signal  = 0x02,
	Reply   = 0x03
};


enum class ReplyType : sf::Uint8 {
	SystemInfo    = 1,
	SignalSuccess = 2
};

struct SignalSuccessReply { };

struct SystemInfoReply {
	sf::Uint64 BootupTime;
	unsigned long NameLength;
	char PCName[16];
};

struct Reply{
	ReplyType Type;
	union {
		SignalSuccessReply SignalSuccessData;
		SystemInfoReply SystemInfoData;
	};
};

enum class RequestType : sf::Uint8 {
	SystemInfo = 1
};

struct Request {
	RequestType Type;
};

enum class SignalType : sf::Uint8 {
	Shutdown = 1,
	Message  = 2
};

constexpr size_t MaxMessageSize = 32;
struct SignalMessage {
	sf::Uint32 Length;
	char Message[MaxMessageSize];
};

struct Signal {
	SignalType Type;
	union {
		SignalMessage SignalMessageData;
	};
};

struct Datagram {
	sf::Uint64   MagicWord;
	DatagramType Type;
	union {
		Request RequestData;
		Signal  SignalData;
		Reply   ReplyData = {};
	};

	Datagram(DatagramType type) {
		MagicWord = ::MagicWord;
		Type = type;
	}

	inline static Datagram SignalDatagram(SignalType type) {
		Datagram result(DatagramType::Signal);
		result.SignalData.Type = type;
		return result;
	}

	friend sf::Packet& operator<<(sf::Packet& packet, const Datagram& datagram) {
		packet << datagram.MagicWord << (sf::Uint8)datagram.Type;
		switch (datagram.Type) {
		case DatagramType::Request:
			packet << (sf::Uint8)datagram.RequestData.Type;
			break;
		case DatagramType::Reply:
			packet << (sf::Uint8)datagram.ReplyData.Type;
			switch (datagram.ReplyData.Type) {
			case ReplyType::SignalSuccess:
				break;
			case ReplyType::SystemInfo:
				packet << datagram.ReplyData.SystemInfoData.BootupTime;
				packet << (sf::Uint32)datagram.ReplyData.SystemInfoData.NameLength;
				packet.append(datagram.ReplyData.SystemInfoData.PCName, datagram.ReplyData.SystemInfoData.NameLength);
				break;
			}
			break;
		case DatagramType::Signal:
			packet << (sf::Uint8)datagram.SignalData.Type;
			switch (datagram.SignalData.Type) {
			case SignalType::Shutdown:
				break;
			case SignalType::Message:
				break;
			}
			break;
		}
		return packet;
	}
	friend sf::Packet& operator>>(sf::Packet& packet, Datagram& datagram) {
		packet >> datagram.MagicWord;
		packet >> (sf::Uint8&)datagram.Type;
		switch(datagram.Type){
		case DatagramType::Request:
			packet >> (sf::Uint8&)datagram.RequestData.Type;
			break;
		case DatagramType::Reply:
			packet >> (sf::Uint8&)datagram.ReplyData.Type;
			switch (datagram.ReplyData.Type) {
			case ReplyType::SignalSuccess:
				break;
			case ReplyType::SystemInfo:
				packet >> datagram.ReplyData.SystemInfoData.BootupTime;
				packet >> (sf::Uint32&)datagram.ReplyData.SystemInfoData.NameLength;
				for (size_t i = 0; i < datagram.ReplyData.SystemInfoData.NameLength && packet >> (sf::Uint8&)datagram.ReplyData.SystemInfoData.PCName[i]; ++i);
				datagram.ReplyData.SystemInfoData.PCName[datagram.ReplyData.SystemInfoData.NameLength] = 0;
				break;
			}
			break;
		case DatagramType::Signal:
			packet >> (sf::Uint8&)datagram.SignalData.Type;
			switch (datagram.SignalData.Type) {
			case SignalType::Shutdown:
				break;
			case SignalType::Message:
				packet >> (sf::Uint32&)datagram.SignalData.SignalMessageData.Length;
				for (size_t i = 0; packet >> (sf::Uint8&)datagram.SignalData.SignalMessageData.Message[i] && i < MaxMessageSize - 1; ++i);
				datagram.SignalData.SignalMessageData.Message[MaxMessageSize - 1] = 0;
				break;
			}
			break;
		}
		return packet;
	}
};
