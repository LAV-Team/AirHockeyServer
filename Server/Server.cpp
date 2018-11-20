#include "Server.hpp"

Server::ServerPtr Server::Create(unsigned short port)
{
	ServerPtr server{ new Server{ port } };
	return server;
}

Server::Server(unsigned short port)
	: isStarted_{ false }
	, service_{}
	, serviceThread_{}
	, acceptor_{ service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port) }
	, lastSessionId_{}
	, sessions_{}
{}

void Server::Start()
{
	if (isStarted_) {
		return;
	}
	isStarted_ = true;

	CreateTransceiver_();
	serviceThread_ = std::thread{ boost::bind(&boost::asio::io_service::run, &service_) };
}

void Server::Stop()
{
	if (!isStarted_) {
		return;
	}
	isStarted_ = false;
	
	for (auto client : clients_) {
		if (client->Sock().is_open()) {
			client->Close();
		}
	}
	service_.stop();
	serviceThread_.join();
}

void Server::CreateTransceiver_()
{
	Client::ClientPtr client{ Client::Create(service_) };
	client->SetErrorHandler(BIND(ErrorHandler_, client, _1));
	client->SetAnswerHandler(BIND(AnswerHandler_, client, _1));
	client->SetCloseHandler(BIND(CloseHandler_, client));

	clients_.push_back(client);
	acceptor_.async_accept(client->Sock(), BIND(AcceptHandler_, client, _1));
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

void Server::AcceptHandler_(Client::ClientPtr client, boost::system::error_code const& error)
{
	if (lastSessionId_.empty()) {
		lastSessionId_ = GenerateSessionId_();
		client->SetSessionId(lastSessionId_);
		sessions_[lastSessionId_].first = client;
		std::cout << client->GetSessionId() << ": First client was connected." << std::endl;
	}
	else {
		client->SetSessionId(lastSessionId_);
		sessions_[lastSessionId_].second = client;
		lastSessionId_.clear();
		std::cout << client->GetSessionId() << ": Second client was connected." << std::endl;
	}

	client->StartReading();
	CreateTransceiver_();
}

void Server::ErrorHandler_(Client::ClientPtr client, boost::system::error_code const& error)
{
	std::cout << client->GetSessionId() << ": " << error.message() << std::endl;
}

void Server::AnswerHandler_(Client::ClientPtr client, std::string const& answer)
{
	try {
		auto session{ sessions_.at(client->GetSessionId()) };
		if (!session.first || !session.first->Sock().is_open()) {
			std::cout << client->GetSessionId() << ": No first client in session!" << std::endl;
			client->Send("No another client in session!");
		}
		else if (!session.second || !session.second->Sock().is_open()) {
			std::cout << client->GetSessionId() << ": No second client in session!" << std::endl;
			client->Send("No another client in session!");
		}
		else if (client == session.first) {
			std::cout << client->GetSessionId() << ": First client sent \"" << answer << "\"" << std::endl;
			session.second->Send(answer);
		}
		else if (client == session.second) {
			std::cout << client->GetSessionId() << ": Second client sent \"" << answer << "\"" << std::endl;
			session.first->Send(answer);
		}
		else {
			std::cout << client->GetSessionId() << ": Unknown client in session!" << std::endl;
			client->Send("You are a strange client!");
		}
	}
	catch(std::out_of_range&) {
		std::cout << client->GetSessionId() << ": Unknown session id!" << std::endl;
		client->Send("You are a strange client!");
	}
}

void Server::CloseHandler_(Client::ClientPtr client)
{
	try {
		auto session{ sessions_.at(client->GetSessionId()) };
		if (client == session.first) {
			std::cout << client->GetSessionId() << ": First client was disconnected!" << std::endl;
			if (session.second && session.second->Sock().is_open()) {
				session.second->Send("Another client was disconnected!");
			}
		}
		else if (client == session.second) {
			std::cout << client->GetSessionId() << ": Second client was disconnected!" << std::endl;
			if (session.first && session.first->Sock().is_open()) {
				session.first->Send("Another client was disconnected!");
			}
		}
		else {
			std::cout << client->GetSessionId() << ": Unknown client was disconnected!" << std::endl;
		}
	}
	catch (std::out_of_range&) {
		std::cout << client->GetSessionId() << ": Unknown session id of disconnected client!" << std::endl;
	}
}
