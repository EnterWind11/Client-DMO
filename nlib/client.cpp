#include "base.h"
#include "asio.hpp"

#include "cClient.h"

#include <thread>

#include "asio/system_error.hpp"
#include "../LibProj/CsFunc/CsMessageBox.h"

//#include "lib_netsockets/Include/json_message.hh"

using namespace asio::ip;

// yeah...
static asio::io_context io_ctx;

cClient::cClient(uint16_t) : enc(false), m_v(0), m_timets(0), m_sock(io_ctx), m_a(false), m_sb(false)
{

}

bool cClient::Connect(const char* ip, uint16_t port)
{
	//CsMessageBoxA(MB_OK, "\n connecting to %s::%d\n", ip, port);
	if (m_a)
	{
		m_sock.cancel();
		m_sock.close();
	}

	m_sock.async_connect(tcp::endpoint(address().from_string(trim(ip)), port), [this](const std::error_code& ec)
		{
			if (ec)
			{
#ifdef DEBUG_NETWORK
				//CsMessageBoxA(MB_OK, "Error while transfering bytes %d", ec);
#endif
				OnDisconnected((char*)"Disconnecting with error 10003\n");
				m_a = false;
			}
			else
			{
				OnConnected();
				StartRead();
			}
		});

	m_a = true;

	return true;
}

void cClient::EnableEncryption(unsigned int version)
{
	// TODO
}

void cClient::DoSend(cPacket& p)
{
	if (m_sb) {

#ifdef DEBUG_NETWORK
		//CsMessageBoxA(MB_OK, "Requests disabled for now");
#endif
		return;
	}

	asio::async_write(m_sock, asio::buffer(p.m_buf.data(), p.real_len), asio::transfer_exactly(p.real_len), [this, &p](const std::error_code& ec, size_t bt)
	{
		p.m_buf.consume(bt);

		if (ec)
		{
#ifdef DEBUG_NETWORK
			//CsMessageBoxA(MB_OK, "Error while transfering bytes %d", ec);
#endif
			OnDisconnected((char*)"10003");
			m_a = false;
		}
	});
}


void cClient::DoDisconnect()
{
	try
	{
		m_sock.shutdown(m_sock.shutdown_send);
	}
	catch (...) {}

	m_a = false;
	OnDisconnected((char*)"10003");
}

void cClient::Stop()
{

}

bool cClient::Bind()
{
	return true;
}

cClient::~cClient()
{
	try
	{
		if (!m_sock.is_open())
			m_sock.close();
	}
	catch (...)
	{
		if (!m_sock.is_open())
			m_sock.close();
	}
}

bool cClient::Run(int unk)
{
	if (m_t.joinable())
		return true;

	std::thread t([]()
		{
			io_ctx.run();
		});

	m_t = std::move(t);
	return true;
}

void cClient::DoExecute()
{

}

void cClient::StartRead()
{
	asio::async_read(m_sock, m_read.m_buf.prepare(2), asio::transfer_exactly(2), [this](asio::error_code ec, std::size_t)
	{
		if (ec)
		{
#ifdef DEBUG_NETWORK
			//CsMessageBoxA(MB_OK, "Error while transfering bytes %d", ec);
#endif
			OnDisconnected((char*)"10003");
			m_a = false;
			return;
		}

		m_read.m_buf.commit(2);

		uint16_t len;
		m_read.pop(len);

		m_read.real_len = len;

		int len2 = len - 2;
	 
		if (len2 > 0) {
			ReadAll(len2);
		}
		else{
			StartRead();
			}
	});
}

void cClient::ReadAll(int b)
{
	asio::async_read(m_sock, m_read.m_buf.prepare(b), asio::transfer_exactly(b), [this](asio::error_code ec, std::size_t bt)
		{
			if (ec)
			{
#ifdef DEBUG_NETWORK
				//CsMessageBoxA(MB_OK, "Error while transfering bytes %d", ec);
#endif
				OnDisconnected((char*)"10003");
				m_a = false;
				return;
			}

			m_read.m_buf.commit(bt);

			m_read.pop(m_sproto);

			OnExecute();

			m_read.m_buf.consume(bt); // force consume everything else

			StartRead();
		});
}


void cClient::push(void* data, size_t len, bool wlen)
{
	m_write.push(data, len, wlen);
}

void cClient::pop(void* data, size_t len, bool wlen)
{
	m_read.pop(data, len, wlen);
}

void cClient::mark(void* data, size_t len)
{
	m_write.mark(data, len); // TODO: what is this?
}

void cClient::endp(uint16_t id)
{
	m_write.endp(id);
}

void cClient::newp(uint16_t id)
{
	m_write.newp(id);
}

