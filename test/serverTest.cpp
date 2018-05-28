#include "netinet/in.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include <iostream>
#include <vector>
#include <errno.h>
#include <ctime>


struct ip_pass_count
//ñòðóêòóðà êëèåíòîâ äëÿ îáðàáîòêè ïàðîëÿ,ip,êîë-âî ïîïûòîê,âðåìÿ áëîêèðîâêè
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



//Ñåðâåð

int main(int argc, char** argv)
{

	// Ñîçäàíèå ñîêåòà ïðîòîêîë UDP
	int serverSocket;
	if ((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		std::cout << "Socket creation error" << std::endl;
		return 1;
	}

	std::cout << "Socket created" << std::endl;
	
	// IP-àäðåñ ñåðâåðà, ïîðò
	sockaddr_in addressInf;
	addressInf.sin_family = AF_INET;
	addressInf.sin_port = htons(56789);
	addressInf.sin_addr.s_addr = htonl(INADDR_ANY);
	//addressInf.sin_addr = htonl(INADDR_ANY);
	//inet_pton(addressInf.sin_family, "192.168.56.1", &addressInf.sin_addr);
    //    inet_aton("10.0.2.15", &addressInf.sin_addr);

	// Ïðèâÿçêà ñîêåòà ê àäðåñó
	if (bind(serverSocket, (sockaddr*) &addressInf, sizeof(addressInf)) == -1)
	{
		std::cout << "Socket binding error" << std::endl;
		//closesocket(serverSocket);
		return 1;
	}

	std::cout << "Socket binded" << std::endl;

	// Ìàññèâ ïîäêëþ÷åííûõ êëèåíòîâ
	std::vector<sockaddr_in> clientsAddresses;
	sockaddr_in clientInfo;
	socklen_t clientSize = sizeof(clientInfo);
	// Áóôåð äëÿ ïîëó÷åííûõ îò êëèåíòà äàííûõ
	char buffer[1024];
	int receivedBytes;
	std::string message;//ñîîáùåíèå êëèåíòà
	std::string mes;//ñîîáùåíèå ñåðâåðà
	std::string passServer = "A1234";//ïàðîëü íà ñåðâåðå

	ip_pass_count ippasscount;
	std::vector<ip_pass_count> clientsStruct; //âåêòîð ñòðóêòóð êëèåíòîâ
	std::cout << "Waiting for clients..." << std::endl;

	// Ñåðâåð æäåò ñîîáùåíèÿ îò êëèåíòîâ
	while (true)
	{
		receivedBytes = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&clientInfo, &clientSize);
		// Ñîîáùåíèå ïîëó÷åíî
		if (receivedBytes > 0)
		{
			std::string passClient = message.assign(buffer);
			passClient = passClient.substr(0, receivedBytes);//îòäåëèòü çíà÷àùóþ ÷àñòü - ïàðîëü

			std::cout << "Client message" << std::endl;
			// Åñëè êëèåíò íå áûë ïîäêëþ÷åí ê ñåðâåðó
			if (!alreadyConnected(clientInfo, clientsAddresses))//äîáàâëåíèå íîâîãî êëèåíòà â âåêòîð
			{
				
				clientsAddresses.push_back(clientInfo);// Íîâûé êëèåíò çàíîñèòñÿ â ìàññèâ
				clientsStruct.push_back(ip_pass_count(clientInfo));
			}
			ip_pass_count* Client_ch = func1(clientInfo, clientsStruct);//óçíàåì òåêóùåãî êëèåíòà èç ñóùåñòâóþùèõ â âåêòîðå (ip)
			time_t rawtime;
			if (Client_ch->countpass > 0)//åùå åñòü ïîïûòêè
			{ 
				//ïðîâåðÿåì ïàðîëü
				if (passServer==passClient)
				{
					//îòïðàâëÿåì îê 
					mes = "Correctly";
					sendto(serverSocket, mes.c_str(), 10, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));//mes.length(),
				}
				else 
				{
					// íå âåðíûé ïàðîëü
					Client_ch->countpass -= 1;
					if (Client_ch->countpass == 0) //íåò ïîïûòîê- â áëîê
					{
						Client_ch->seconds = time(&rawtime);
						//ïðèñûëàåì áëîê
						//mes = "Password is not correct -block";
							mes = "Blocked.";
						sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
					}
					else 
					{
						//íå âåðíûé ïàðîëü
						//mes = "Password is not correct";
						mes = "Incorrectly";
						sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
					}
				}
			}
				
			else
			{
				//áîëüøå íåò ïîïûòîê
				//block client;
				if ((time(&rawtime) - Client_ch->seconds) < 120)//ñ íà÷àëà áëîêèðîâêè ïðîøëî ìåíåå 2-õ ìèí
				{
					//âñå åùå áëîêèðîâàí
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
						//îòïðàâëÿåì îê 
						//mes = "Correct password";
						mes = "Correctly";
						sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
					}
					else
					{
						//íå âåðíûé ïàðîëü
						//mes = "Password is not correct -block";
						mes = "Blocked.";
						sendto(serverSocket, mes.c_str(), 11, 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
						Client_ch->countpass--;
					}
				}
			}
		}
	}

	//closesocket(serverSocket);
	
	return 0;
}

ip_pass_count* func1(sockaddr_in& address, std::vector<ip_pass_count>& clients)//ïîëó÷àåì ip êëèåíòà
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i].clientInfo.sin_addr.s_addr == address.sin_addr.s_addr)
			return &clients[i];
	}
}

bool alreadyConnected(sockaddr_in& clientInfo, std::vector<sockaddr_in>& connectedAddresses)
{
	//for (auto client : connectedAddresses)
	for (size_t i = 0; i < connectedAddresses.size(); ++i)
	{
		if (connectedAddresses[i].sin_addr.s_addr == clientInfo.sin_addr.s_addr)
			return true;
	}

	return false;
}
