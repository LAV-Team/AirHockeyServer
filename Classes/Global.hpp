#ifndef __GLOBAL_INCLUDED__
#define __GLOBAL_INCLUDED__

#include <string>

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

class Receiver;
class Transmitter;
class Transceiver;

typedef boost::shared_ptr<Receiver> ReceiverPtr;
typedef boost::shared_ptr<Transmitter> TransmitterPtr;
typedef boost::shared_ptr<Transceiver> TransceiverPtr;

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
typedef boost::function<void(boost::system::error_code const&)> OnErrorHandler;
typedef boost::function<void(std::string const&)> OnAnswerHandler;
typedef boost::function<void()> OnCloseHandler;

static std::string const NO_SESSION{ "SN" };
static std::string const UNKNOWN_SESSION{ "SU" };
static std::string const SESSION_BEGIN{ "SB" };
static std::string const SESSION_END{ "SE" };
static std::string const WAIT_SESSION{ "SW" };
static std::string const STOP_WAITING_SESSION{ "SF" };
static std::string const STOP_SESSION{ "ST" };

static char const COMMAND_END{ '\x01' };
static char const TRANSMITTER_END{ '\x02' };
static char const TRANSCEIVER_END{ '\x03' };

static size_t const BUFFER_LENGTH{ 256U };
static size_t const SESSION_ID_LENGTH{ 16U };
static size_t const SHORT_SESSION_ID_LENGTH{ 7U };

#define BIND(a, ...) boost::bind(&SelfType::a, shared_from_this(), __VA_ARGS__)
#define IS_COMMAND_END(ch) ((ch) == COMMAND_END)
#define IS_STREAM_END(ch) ((ch) == TRANSMITTER_END || (ch) == TRANSCEIVER_END)
#define IS_END(ch) ((ch) == COMMAND_END || (ch) == TRANSMITTER_END || (ch) == TRANSCEIVER_END)

#endif // __GLOBAL_INCLUDED__
