#include <iostream>
#include "Server.hpp"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	std::string buffer{};

	unsigned short port{ 0U };
	std::cout << "Enter port (blank for 4444): ";
	std::getline(std::cin, buffer);
	port = buffer.empty() ? 4444 : static_cast<unsigned short>(std::stoul(buffer));

	HockeyNet::ServerPtr server{ HockeyNet::Server::Create(port) };
	server->Start();
	std::cout << "Server started on " << port << " port..." << std::endl;
	while (std::getline(std::cin, buffer)) {
		if (buffer == "/stop") {
			std::cout << "Stopping..." << std::endl;
			break;
		}
		else if (!buffer.empty()) {
			std::cout << "Unknown command!" << std::endl;
		}
	}
	server->Stop();

	system("pause");
}
