#include "Server.hpp"

Server::SharedPtr Server::Create(unsigned short port)
{
	SharedPtr server{ new Server{ port } };
	return server;
}

Server::Server(unsigned short port)
	: isStarted_{ false }
	, service_{}
	, acceptor_{ service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port) }
{}

void Server::Start()
{
	if (isStarted_) {
		return;
	}
	isStarted_ = true;

	CreateTransceiver_();
	service_.run();
}

void Server::Stop()
{
	if (!isStarted_) {
		return;
	}
	isStarted_ = false;

	service_.stop();
}

void Server::CreateTransceiver_()
{
	Transceiver::SharedPtr client = Transceiver::Create(service_);
	client->SetErrorHandler(BIND_WITH_2_ARGS(ErrorHandler_, client, _1));
	client->SetAnswerHandler(BIND_WITH_2_ARGS(AnswerHandler_, client, _1));
	acceptor_.async_accept(client->Sock(), BIND_WITH_2_ARGS(AcceptHandler_, client, _1));
}

std::string Server::GenerateSessionId_()
{
	std::string result{};
	for (size_t i{ 0U }; i < SESSION_ID_LENGTH; ++i) {
		switch (rand() % 5)
		{
		case 0:
		case 1:
			result += 'A' + rand() % 26;
			break;
		case 2:
		case 3:
			result += 'a' + rand() % 26;
			break;
		case 4:
			result += '0' + rand() % 10;
			break;
		}
	}
	return result;
}

void Server::AcceptHandler_(Transceiver::SharedPtr client, boost::system::error_code const& error)
{
	if (lastSessionId_.empty()) {
		lastSessionId_ = GenerateSessionId_();
		client->SetSessionId(lastSessionId_);
		clients_[lastSessionId_].first = client;
		std::cout << client->GetSessionId() << ": First client." << std::endl;
	}
	else {
		client->SetSessionId(lastSessionId_);
		clients_[lastSessionId_].second = client;
		lastSessionId_.clear();
		std::cout << client->GetSessionId() << ": Second client." << std::endl;
	}

	client->StartReading();
	CreateTransceiver_();
}

void Server::ErrorHandler_(Transceiver::SharedPtr client, boost::system::error_code const& error)
{
	std::cout << client->GetSessionId() << ": " << error.message() << std::endl;
}

void Server::AnswerHandler_(Transceiver::SharedPtr client, std::string const& answer)
{
	auto session{ clients_.at(client->GetSessionId()) };
	if (!session.first || !session.first->Sock().is_open()) {
		std::cout << client->GetSessionId() << ": No first user in session!" << std::endl;
		client->Send("No another user in session!");
	}
	else if (!session.second || !session.second->Sock().is_open()) {
		std::cout << client->GetSessionId() << ": No second user in session!" << std::endl;
		client->Send("No another user in session!");
	}
	else if (client == session.first) {
		std::cout << client->GetSessionId() << ": First user sent \"" << answer << "\"" << std::endl;
		session.second->Send(answer);
	}
	else if (client == session.second) {
		std::cout << client->GetSessionId() << ": Second user sent \"" << answer << "\"" << std::endl;
		session.first->Send(answer);
	}
	else {
		std::cout << client->GetSessionId() << ": Unknown client in session!" << std::endl;
		client->Send("You are a strange client!");
	}
}
