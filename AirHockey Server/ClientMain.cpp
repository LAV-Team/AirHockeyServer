#include "Client.hpp"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 4444);
	Client::SharedPtr client{ Client::Create(ep) };
	client->Start();
	system("pause");
}
