#include "netinet/in.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include <iostream>
#include <string>
#include "gtest/gtest.h"


//bool makeSocket(int* socket)
bool makeSocket(int* sock)
{
	//clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	*sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if (*sock < 0)
		return false;
	return true;
}


TEST(Client, CreateSocket)
{
	int Socket;
	ASSERT_TRUE(makeSocket(&Socket));
}

TEST(Client, WrongPass)
{
	int Socket;
	//makeSocket(Socket);
	makeSocket(&Socket);
	
	sockaddr_in connectionAddress;
	connectionAddress.sin_family = AF_INET;
	connectionAddress.sin_port = htons(56789);
	//connectionAddress.sin_addr = htonl(INADDR_LOOPBACK);
	connectionAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	std::string totallyWrongPassword = "notcorrectpasssorry";
	std::string response;
	char servResponse[1111];
	
	int responseLen = -1;
	
	sendto(Socket, totallyWrongPassword.c_str(), totallyWrongPassword.length(), 0, (struct sockaddr*) &connectionAddress, sizeof(connectionAddress));
	
	responseLen = recvfrom(Socket, servResponse, sizeof(servResponse), 0, NULL, NULL);
	response.assign(servResponse);
	response = response.substr(0, responseLen);
	ASSERT_TRUE(response == "Incorrectly");
}

TEST(Client, CorrectPass)
{
	int Socket;
	//makeSocket(Socket);
	makeSocket(&Socket);
	
	sockaddr_in connectionAddress;
	connectionAddress.sin_family = AF_INET;
	connectionAddress.sin_port = htons(56789);
	//connectionAddress.sin_addr = htonl(INADDR_LOOPBACK);
	connectionAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	std::string totallyWrongPassword = "A1234";
	std::string response;
	char servResponse[1111];
	
	int responseLen = -1;
	
	sendto(Socket, totallyWrongPassword.c_str(), totallyWrongPassword.length(), 0, (struct sockaddr*) &connectionAddress, sizeof(connectionAddress));
	
	responseLen = recvfrom(Socket, servResponse, sizeof(servResponse), 0, NULL, NULL);
	response.assign(servResponse);
	response = response.substr(0, responseLen);
	ASSERT_TRUE(response == "Correctly");
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	
}

	// Клиент ожидает ответ от сервера, данные с сервера выводятся в консоль
	while (true)
	{

                 std::string pas;
	         std::cout << "Input password: " << std::endl;
	         std::cin >> pas;
	         std::cout << std::endl;

	         //клиент посылает серверу пароль, одновременно оповещая его о подключении
	         connect = sendto(clientSocket, pas.c_str(), pas.length(), 0, (struct sockaddr*) &connectionAddress, sizeof(connectionAddress));

		int response = recvfrom(clientSocket, buf, 11, 0, (struct sockaddr*) &connectionAddress, &connectionSize);
		if (response > 0)
			std::cout << buf << std::endl;
	}

	return 0;
}
