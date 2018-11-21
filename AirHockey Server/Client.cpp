#include "Client.hpp"

Client::SharedPtr Client::Create(boost::asio::ip::tcp::endpoint ep)
{
	SharedPtr client{ new Client{} };
	client->transceiver_->SetErrorHandler(boost::bind(&Client::ErrorHandler_, client, _1));
	client->transceiver_->SetAnswerHandler(boost::bind(&Client::AnswerHandler_, client, _1));
	client->transceiver_->Connect(ep);
	return client;
}

Client::Client()
	: isStarted_{ false }
	, service_{}
	, transceiver_{ Transceiver::Create(service_) }
{}

void Client::Start()
{
	if (isStarted_) {
		return;
	}
	isStarted_ = true;

	transceiver_->StartReading();
	StartAsyncSending_();
	service_.run();
}

void Client::Stop()
{
	if (!isStarted_) {
		return;
	}
	isStarted_ = false;

	service_.stop();
}

void Client::StartSending_()
{
	std::string message{};
	while (std::getline(std::cin, message) && message != "/stop") {
		transceiver_->Send(message);
	}
	transceiver_->Cancel();
}

void Client::StartAsyncSending_()
{
	sendingThread_ = std::thread{ BIND(StartSending_) };
}

void Client::ErrorHandler_(boost::system::error_code const& error)
{
	std::cout << error.message() << std::endl;
}

void Client::AnswerHandler_(std::string const& answer)
{
	std::cout << answer << std::endl;
}
