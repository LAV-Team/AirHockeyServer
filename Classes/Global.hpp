#ifndef __GLOBAL_INCLUDED__
#define __GLOBAL_INCLUDED__

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

#define BIND(a) boost::bind(&SelfType::a, shared_from_this())
#define BIND_WITH_1_ARG(a, b) boost::bind(&SelfType::a, shared_from_this(), b)
#define BIND_WITH_2_ARGS(a, b, c) boost::bind(&SelfType::a, shared_from_this(), b, c)
#define BIND_WITH_3_ARGS(a, b, c, d) boost::bind(&SelfType::a, shared_from_this(), b, c, d)

static size_t const BUFFER_LENGTH = 256U;
static char const COMMAND_END = ';';
static char const MESSAGE_END = '\n';

static size_t const SESSION_ID_LENGTH = 8U;

#endif // __GLOBAL_INCLUDED__
