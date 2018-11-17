#include "Network.hpp"

void ErrorHandler_(boost::system::error_code const& error)
{
	std::cout << error.message() << std::endl;
}

void AnswerHandler_(std::string const& answer)
{
	std::cout << answer << std::endl;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	Network::SharedPtr network{ Network::Create() };
	network->SetErrorHandler(ErrorHandler_);
	network->SetAnswerHandler(AnswerHandler_);

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 4444);
	if (network->Connect(ep)) {
		network->StartReading();
		std::string message{};
		while (std::getline(std::cin, message) && message != "/stop") {
			network->Send(message);
		}
		network->Disconnect();
	}
	else {
		std::cout << "Connection error!" << std::endl;
	}
	system("pause");
}
