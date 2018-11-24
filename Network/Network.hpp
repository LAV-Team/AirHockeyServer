#ifndef __NETWORK_INCLUDED__
#define __NETWORK_INCLUDED__

#include "../Classes/General.hpp"
#include "../Classes/Transceiver.hpp"

namespace HockeyNet
{
	class Network;
	typedef boost::shared_ptr<Network> NetworkPtr;

	class Network
		: public boost::enable_shared_from_this<Network>
		, boost::noncopyable
	{
	public:
		static NetworkPtr Create();
		void SetErrorHandler(OnErrorHandler onErrorHandler);
		void SetAnswerHandler(OnAnswerHandler onAnswerHandler);
		void SetCloseHandler(OnCloseHandler onCloseHandler);

		bool IsConnected();

		bool Connect(boost::asio::ip::tcp::endpoint const& ep);
		void Disconnect();

		void StartReading();
		void Send(std::string const& message);

	private:
		typedef Network SelfType;

		bool isStarted_;
		boost::asio::io_service service_;
		std::thread serviceThread_;
		TransceiverPtr transceiver_;

		Network();
	};
}

#endif // __NETWORK_INCLUDED__
