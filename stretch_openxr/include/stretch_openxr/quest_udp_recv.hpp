
/*
 * =====================================================================================
 *
 * Filename:  quest_udp_recv.hpp
 *
 * Description:  The header for your UDP receiver class (for the robot).
 *
 * =====================================================================================
 */
#pragma once

#include "quest_udp_common.hpp" // Include the shared data structure
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif


class QuestUdpReceiver {
public:
    // Constructor: Sets up the socket to listen on a specific port.
    QuestUdpReceiver(int port);

    // Destructor: Cleans up and closes the network connection.
    ~QuestUdpReceiver();

    // Attempts to receive joystick data. Returns true if data was received.
    // The received data is written into the 'data' parameter.
    bool receiveData(JoystickData& data);

private:
    #ifdef _WIN32
        SOCKET m_socket_fd;
        WSADATA m_wsa_data;
    #else
        int m_socket_fd;
    #endif

    // Helper function to initialize the socket.
    bool init_socket(int port);
};