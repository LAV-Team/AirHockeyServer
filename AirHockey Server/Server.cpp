#include <iostream>
#include <string>
#include <map>
#include "Transceiver.hpp"

class Server
	: public boost::enable_shared_from_this<Server>
	, boost::noncopyable
{
public:
	typedef boost::shared_ptr<Server> SharedPtr;

	static SharedPtr Create(unsigned short port = 4444)
	{
		SharedPtr server{ new Server{ port } };
		return server;
	}

	void ErrorHandler(Transceiver::SharedPtr client, boost::system::error_code const& error)
	{
		std::cout << client->GetSessionId() << ": " << error.message() << std::endl;
	}

	void AnswerHandler(Transceiver::SharedPtr client, std::string const& answer)
	{
		auto session{ clients_.at(client->GetSessionId()) };
		if (!session.first || !session.second || !session.first->Sock().is_open() || !session.second->Sock().is_open()) {
			std::cout << client->GetSessionId() << ": No second user in session!" << std::endl;
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
		}
	}

	std::string GenerateSessionId()
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

	void CreateTransceiver()
	{
		Transceiver::SharedPtr client = Transceiver::Create(service_);
		client->SetErrorHandler(BIND_WITH_2_ARGS(ErrorHandler, client, _1));
		client->SetAnswerHandler(BIND_WITH_2_ARGS(AnswerHandler, client, _1));
		acceptor_.async_accept(client->Sock(), BIND_WITH_2_ARGS(HandleAccept, client, _1));
	}

	void HandleAccept(Transceiver::SharedPtr client, boost::system::error_code const& error)
	{
		if (lastSessionId_.empty()) {
			lastSessionId_ = GenerateSessionId();
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
		CreateTransceiver();
	}

	void Start()
	{
		if (isStarted_) {
			return;
		}
		isStarted_ = true;
		CreateTransceiver();
		service_.run();
	}

	void Stop()
	{
		if (!isStarted_) {
			return;
		}
		isStarted_ = false;
		acceptor_.close();
	}

private:
	typedef Server SelfType;

	boost::asio::io_service service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	bool isStarted_;

	std::string lastSessionId_;
	std::map<std::string, std::pair<Transceiver::SharedPtr, Transceiver::SharedPtr>> clients_;

	Server(unsigned short port)
		: service_{}
		, acceptor_{ service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port) }
		, isStarted_{ false }
	{}
};

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");

	Server::SharedPtr server{ Server::Create(4444) };
	server->Start();
}