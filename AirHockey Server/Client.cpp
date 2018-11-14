#include <iostream>
#include <string>
#include "Transceiver.hpp"

void ErrorHandler(boost::system::error_code const& error)
{
	std::cout << error.message() << std::endl;
}

void AnswerHandler(std::string const& answer)
{
	std::cout << answer << std::endl;
}

void Input(Transceiver::SharedPtr transceiver)
{
	std::string message{};
	while (std::getline(std::cin, message) && message != "/stop") {
		transceiver->Send(message);
	}
	transceiver->Cancel();
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	boost::asio::io_service service;
	
	Transceiver::SharedPtr transceiver{ Transceiver::Create(service) };
	transceiver->SetErrorHandler(ErrorHandler);
	transceiver->SetAnswerHandler(AnswerHandler);
	
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 4444);
	transceiver->Connect(ep);
	
	transceiver->StartReading();
	std::thread input{ boost::bind(Input, transceiver) };
	service.run();

	input.join();
	system("pause");
}