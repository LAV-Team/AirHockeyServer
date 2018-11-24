#include <iostream>
#include "Network.hpp"

void OnError(boost::system::error_code const& error)
{
	std::cout << error.message() << std::endl;
}

void OnAnswer(std::string const& answer)
{
	std::cout << answer << std::endl;
}

void OnClose()
{
	std::cout << "Server does't want to talk with you :c" << std::endl;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	std::string buffer{};

	std::string address{};
	std::cout << "Enter IP (blank for 127.0.0.1): ";
	std::getline(std::cin, buffer);
	address = buffer.empty() ? "127.0.0.1" : buffer;

	unsigned short port{ 0U };
	std::cout << "Enter port (blank for 4444): ";
	std::getline(std::cin, buffer);
	port = buffer.empty() ? 4444 : static_cast<unsigned short>(std::stoul(buffer));

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(address), port);

	HockeyNet::NetworkPtr network{ HockeyNet::Network::Create() };
	network->SetErrorHandler(OnError);
	network->SetAnswerHandler(OnAnswer);
	network->SetCloseHandler(OnClose);
	if (network->Connect(ep)) {
		std::cout << "Connected to " << address << ":" << port << "..." << std::endl;
		network->StartReading();
		while (std::getline(std::cin, buffer) && network->IsConnected()) {
			if (buffer == "/exit") {
				std::cout << "Stopping..." << std::endl;
				break;
			}
			else if (buffer == "/wait") {
				network->Send(HockeyNet::WAIT_SESSION);
			}
			else if (buffer == "/stop") {
				network->Send(HockeyNet::STOP_WAITING_SESSION);
			}
			else if (buffer == "/cancel") {
				network->Send(HockeyNet::STOP_SESSION);
			}
			else if (!buffer.empty()) {
				network->Send(buffer);
			}
		}
		network->Disconnect();
	}
	else {
		std::cout << "Connection error!" << std::endl;
	}

	system("pause");
}
