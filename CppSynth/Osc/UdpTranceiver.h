#ifndef UDP_H
#define UDP_H

#include <vector>
#include <stdint.h>

#include <boost/asio.hpp>
#include <boost/array.hpp>
using boost::asio::ip::udp;

class UdpTranceiver
{
private:
	boost::asio::io_service ioServiceSend;
	udp::endpoint sendEndpoint;
	udp::socket* sendSocket;

	boost::asio::io_service ioServiceReceive;
	udp::socket* receiveSocket;
	boost::array<uint8_t, 64000> recveiveBuffer;

public:
	UdpTranceiver(int receivePort, int sendPort);
	UdpTranceiver(int receivePort, int sendPort, std::string sendIp);
	~UdpTranceiver();

	std::vector<uint8_t> Receive();
	void Send(std::vector<uint8_t> message);

};


#endif