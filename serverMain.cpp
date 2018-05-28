#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <errno.h>
#include <ctime>


struct ip_pass_count
//��������� �������� ��� ��������� ������,ip,���-�� �������,����� ����������
{
	ip_pass_count() = default;
	ip_pass_count(sockaddr_in clientInfo) : clientInfo(clientInfo)
	{
		countpass = 3;
	}
	sockaddr_in clientInfo;
	int countpass = 3;
	time_t seconds;
};

bool alreadyConnected(sockaddr_in&, std::vector<sockaddr_in>&);
ip_pass_count* func1(sockaddr_in&, std::vector<ip_pass_count>&);



//������

int main(int argc, char** argv)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		std::cout << "WinSock initialization error" << std::endl;
		return 1;
	}
	
	std::cout << "WinSock initialized" << std::endl;

	// �������� ������ �������� UDP
	SOCKET serverSocket;
	if ((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		std::cout << "Socket creation error" << std::endl;
		WSACleanup();
		return 1;
	}

	std::cout << "Socket created" << std::endl;
	
	// IP-����� �������, ����
	sockaddr_in addressInf;
	addressInf.sin_family = AF_INET;
	addressInf.sin_port = htons(56789);
	inet_pton(addressInf.sin_family, "192.168.56.1", &addressInf.sin_addr);

	// �������� ������ � ������
	if (bind(serverSocket, (SOCKADDR*) &addressInf, sizeof(addressInf)) == SOCKET_ERROR)
	{
		std::cout << "Socket binding error" << std::endl;
		closesocket(serverSocket);
		return 1;
	}

	std::cout << "Socket binded" << std::endl;

	// ������ ������������ ��������
	std::vector<sockaddr_in> clientsAddresses;
	sockaddr_in clientInfo;
	int clientSize = sizeof(clientInfo);
	// ����� ��� ���������� �� ������� ������
	char buffer[1024];
	int receivedBytes;
	std::string message;//��������� �������
	std::string mes;//��������� �������
	std::string passServer = "A1234";//������ �� �������

	ip_pass_count ippasscount;
	std::vector<ip_pass_count> clientsStruct; //������ �������� ��������
	std::cout << "Waiting for clients..." << std::endl;

	// ������ ���� ��������� �� ��������
	while (true)
	{
		receivedBytes = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&clientInfo, &clientSize);
		// ��������� ��������
		if (receivedBytes > 0)
		{
			std::string passClient = message.assign(buffer);
			passClient = passClient.substr(0, receivedBytes);//�������� �������� ����� - ������

			std::cout << "Client message" << std::endl;
			// ���� ������ �� ��� ��������� � �������
			if (!alreadyConnected(clientInfo, clientsAddresses))//���������� ������ ������� � ������
			{
				
				clientsAddresses.push_back(clientInfo);// ����� ������ ��������� � ������
				clientsStruct.push_back(ip_pass_count(clientInfo));
			}
			ip_pass_count* Client_ch = func1(clientInfo, clientsStruct);//������ �������� ������� �� ������������ � ������� (ip)
			time_t rawtime;
			if (Client_ch->countpass > 0)//��� ���� �������
			{ 
				//��������� ������
				if (passServer==passClient)
				{
					//���������� �� 
					mes = "Correctly";
					sendto(serverSocket, mes.c_str(), 10, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));//mes.length(),
				}
				else 
				{
					// �� ������ ������
					Client_ch->countpass -= 1;
					if (Client_ch->countpass == 0) //��� �������- � ����
					{
						Client_ch->seconds = time(&rawtime);
						//��������� ����
						//mes = "Password is not correct -block";
							mes = "Blocked.";
						sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
					}
					else 
					{
						//�� ������ ������
						//mes = "Password is not correct";
						mes = "Incorrectly";
						sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
					}
				}
			}
				
			else
			{
				//������ ��� �������
				//block client;
				if ((time(&rawtime) - Client_ch->seconds) < 120)//� ������ ���������� ������ ����� 2-� ���
				{
					//��� ��� ����������
					//mes = "Block- Try it later";
					mes = "Blocked.";
					sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
				}
				else
				{
					Client_ch->seconds = 0;
					Client_ch->countpass = 3;
					if (passServer == passClient)
					{
						//���������� �� 
						//mes = "Correct password";
						mes = "Correctly";
						sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
					}
					else
					{
						//�� ������ ������
						//mes = "Password is not correct -block";
						mes = "Blocked.";
						sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
						Client_ch->countpass--;
					}
				}
			}
		}
	}

	closesocket(serverSocket);
	
	return 0;
}

ip_pass_count* func1(sockaddr_in& address, std::vector<ip_pass_count>& clients)//�������� ip �������
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i].clientInfo.sin_addr.S_un.S_addr == address.sin_addr.S_un.S_addr)
			return &clients[i];
	}
}

bool alreadyConnected(sockaddr_in& clientInfo, std::vector<sockaddr_in>& connectedAddresses)
{
	for (auto client : connectedAddresses)
	{
		if (client.sin_addr.S_un.S_addr == clientInfo.sin_addr.S_un.S_addr)
			return true;
	}

	return false;
}