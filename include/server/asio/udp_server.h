/*!
    \file udp_server.h
    \brief UDP server definition
    \author Ivan Shynkarenka
    \date 22.12.2016
    \copyright MIT License
*/

#ifndef CPPSERVER_ASIO_UDP_SERVER_H
#define CPPSERVER_ASIO_UDP_SERVER_H

#include "service.h"

namespace CppServer {
namespace Asio {

//! UDP server
/*!
    UDP server is used to send or multicast datagrams to UDP endpoints.

    Thread-safe.
*/
class UDPServer : public std::enable_shared_from_this<UDPServer>
{
public:
    //! Initialize UDP server with a given Asio service, protocol and port number
    /*!
        \param service - Asio service
        \param protocol - Protocol type
        \param port - Port number
    */
    explicit UDPServer(std::shared_ptr<Service> service, InternetProtocol protocol, int port);
    //! Initialize UDP server with a given Asio service, IP address and port number
    /*!
        \param service - Asio service
        \param address - IP address
        \param port - Port number
    */
    explicit UDPServer(std::shared_ptr<Service> service, const std::string& address, int port);
    //! Initialize UDP server with a given UDP endpoint
    /*!
        \param service - Asio service
        \param endpoint - Server UDP endpoint
    */
    explicit UDPServer(std::shared_ptr<Service> service, const asio::ip::udp::endpoint& endpoint);
    UDPServer(const UDPServer&) = delete;
    UDPServer(UDPServer&&) = default;
    virtual ~UDPServer() { Stop(); }

    UDPServer& operator=(const UDPServer&) = delete;
    UDPServer& operator=(UDPServer&&) = default;

    //! Get the Asio service
    std::shared_ptr<Service>& service() noexcept { return _service; }
    //! Get the server endpoint
    asio::ip::udp::endpoint& endpoint() noexcept { return _endpoint; }
    //! Get the server multicast endpoint
    asio::ip::udp::endpoint& multicast_endpoint() noexcept { return _multicast_endpoint; }

    //! Is the service started?
    bool IsStarted() const noexcept { return _started; }

    //! Start the server
    /*!
        \return 'true' if the server was successfully started, 'false' if the server failed to start
    */
    bool Start();
    //! Stop the server
    /*!
        \return 'true' if the server was successfully stopped, 'false' if the server is already stopped
    */
    bool Stop();

    //! Setup multicast endpoint for the UDP server with a given IP address and port number
    /*!
        \param address - IP address
        \param port - Port number
    */
    void SetupMulticastEndpoint(const std::string& address, int port);
    //! Setup multicast endpoint with a given UDP endpoint
    /*!
        \param endpoint - UDP endpoint
    */
    void SetupMulticastEndpoint(const asio::ip::udp::endpoint& endpoint);

    //! Multicast a datagram to the prepared mulicast endpoint
    /*!
        \param buffer - Datagram buffer to send
        \param size - Datagram buffer size
        \return Count of pending bytes in the send buffer
    */
    size_t Multicast(const void* buffer, size_t size);

    //! Send a datagram into the given endpoint
    /*!
        \param endpoint - Endpoint to send
        \param buffer - Datagram buffer to send
        \param size - Datagram buffer size
        \return Count of pending bytes in the send buffer
    */
    size_t Send(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size);

protected:
    //! Handle server started notification
    virtual void onStarted() {}
    //! Handle server stopped notification
    virtual void onStopped() {}

    //! Handle datagram received notification
    /*!
        Notification is called when another datagram was received from
        some endpoint.

        \param endpoint - Received endpoint
        \param buffer - Received datagram buffer
        \param size - Received datagram buffer size
    */
    virtual void onReceived(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size) {}
    //! Handle datagram sent notification
    /*!
        Notification is called when a datagram was sent to the client.

        This handler could be used to send another datagram to the client
        for instance when the pending size is zero.

        \param endpoint - Endpoint of sent datagram
        \param sent - Size of sent datagram buffer
        \param pending - Size of pending datagram buffer
    */
    virtual void onSent(const asio::ip::udp::endpoint& endpoint, size_t sent, size_t pending) {}

    //! Handle error notification
    /*!
        \param error - Error code
        \param category - Error category
        \param message - Error message
    */
    virtual void onError(int error, const std::string& category, const std::string& message) {}

private:
    // Asio service
    std::shared_ptr<Service> _service;
    // Server endpoint & socket
    asio::ip::udp::endpoint _endpoint;
    asio::ip::udp::socket _socket;
    std::atomic<bool> _started;
    // Multicast & receive endpoint
    asio::ip::udp::endpoint _multicast_endpoint;
    asio::ip::udp::endpoint _recive_endpoint;
    // Receive buffer
    std::mutex _send_lock;
    std::vector<uint8_t> _recive_buffer;
    std::vector<uint8_t> _send_buffer;
    bool _reciving;
    bool _sending;

    static const size_t CHUNK = 8192;

    //! Try to receive datagram
    void TryReceive();
    //! Try to send data
    void TrySend(const asio::ip::udp::endpoint& endpoint, size_t size);
};

/*! \example udp_chat_server.cpp UDP chat server example */

} // namespace Asio
} // namespace CppServer

#include "udp_server.inl"

#endif // CPPSERVER_ASIO_UDP_SERVER_H
