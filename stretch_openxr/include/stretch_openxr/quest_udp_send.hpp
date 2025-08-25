
/*
 * =====================================================================================
 *
 * Filename:  quest_udp_send.hpp
 *
 * Description:  The header for your UDP publisher class (for the Quest).
 *
 * =====================================================================================
 */
#pragma once

#include "quest_udp_common.hpp" // Include the shared data structure
#include <string>

// Platform-specific includes for networking (handles Android/Linux)
#ifdef _WIN32
    // For Windows
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") // Link with the Winsock library
#else
    // For Linux/Android
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif


class QuestUdpSender {
public:
    // Constructor: Sets up the connection to the robot's IP and port.
    QuestUdpSender(const std::string& robot_ip, int port);

    // Destructor: Cleans up and closes the network connection.
    ~QuestUdpSender();

    // Sends the joystick data to the robot.
    bool sendJoystickData(float lx, float ly, float rx, float ry);

private:
    #ifdef _WIN32
        SOCKET m_socket_fd;
        WSADATA m_wsa_data;
    #else
        int m_socket_fd;
    #endif

    struct sockaddr_in m_server_addr; // The robot's address info

    // Helper function to initialize the socket.
    bool init_socket(const std::string& robot_ip, int port);
};

