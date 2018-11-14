#include "Server.hpp"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	Server::SharedPtr server{ Server::Create(4444) };
	server->Start();
	system("pause");
}
