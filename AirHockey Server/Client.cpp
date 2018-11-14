#include <iostream>
#include "Transceiver.hpp"

void ErrorHandler(boost::system::error_code const& error)
{
	std::cout << error.message() << std::endl;
}

void AnswerHandler(std::string const& answer)
{
	std::cout << answer << std::endl;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	boost::asio::io_service service_;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 4444);
	
	Transceiver::SharedPtr transceiver{ Transceiver::Create(service_) };
	transceiver->SetErrorHandler(ErrorHandler);
	transceiver->SetAnswerHandler(AnswerHandler);
	transceiver->Connect(ep);
	transceiver->StartReading();

	char const* messages[]{ "Test 1111111111", "Test 2", "Test 333", "Test 4", "Test 5", "Test 6", 0 };
	for (char const** message = messages; *message; ++message) {
		transceiver->Send(*message);
	}
	transceiver->Send("Test 77777777777777777777777777777777");
	transceiver->Send("Test 8");

	service_.run();
	transceiver->Cancel();
	system("pause");
}