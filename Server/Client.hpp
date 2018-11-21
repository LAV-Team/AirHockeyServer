#ifndef __CLIENT_INCLUDED__
#define __CLIENT_INCLUDED__

#include "../Classes/Transceiver.hpp"

struct Client
{
public:
	typedef boost::shared_ptr<Client> ClientPtr;
	static ClientPtr Create(boost::asio::io_service& service);
	void SetErrorHandler(OnErrorHandler onErrorHandler);
	void SetAnswerHandler(OnAnswerHandler onAnswerHandler);
	void SetCloseHandler(OnCloseHandler onCloseHandler);

	TransceiverPtr GetTransceiver();
	std::string GetSessionId() const;
	std::string GetShortSessionId() const;
	ClientPtr GetAnotherClient() const;

	void SetSessionId(std::string sessionId);
	void SetAnotherClient(ClientPtr anotherClient);

	void ClearSessionId();
	void ClearAnotherClient();

	boost::asio::ip::tcp::socket& Sock();
	bool IsOpen() const;
	void StartReading();
	void Send(std::string const& message);
	void Close();

private:
	TransceiverPtr transceiver_;
	std::string sessionId_;
	std::string shortSessionId_;
	ClientPtr anotherClient_;

	Client(boost::asio::io_service& service);
};

#endif // __CLIENT_INCLUDED__
