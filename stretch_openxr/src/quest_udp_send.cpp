/*
 * =====================================================================================
 *
 * Filename:  quest_udp_send.cpp (Your quest_udp_send.cpp)
 *
 * Description:  The implementation for your UDP publisher class.
 *
 * =====================================================================================
 */
#include "quest_udp_send.hpp"
#include <iostream>
#include <cstring> // For memset
#include <chrono>  // For timestamp

QuestUdpSender::QuestUdpSender(const std::string& robot_ip, int port) : m_socket_fd(-1) {
    #ifdef _WIN32
        m_socket_fd = INVALID_SOCKET;
    #else
        m_socket_fd = -1;
    #endif
    
    if (!init_socket(robot_ip, port)) {
        std::cerr << "Error: Failed to initialize UDP sender socket." << std::endl;
    }
}

QuestUdpSender::~QuestUdpSender() {
    #ifdef _WIN32
        if (m_socket_fd != INVALID_SOCKET) {
            closesocket(m_socket_fd);
            WSACleanup();
        }
    #else
        if (m_socket_fd != -1) {
            close(m_socket_fd);
        }
    #endif
}

bool QuestUdpSender::init_socket(const std::string& robot_ip, int port) {
    #ifdef _WIN32
        if (WSAStartup(MAKEWORD(2, 2), &m_wsa_data) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            return false;
        }
    #endif
    
    m_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    #ifdef _WIN32
        if (m_socket_fd == INVALID_SOCKET) {
            std::cerr << "Error: Cannot create publisher socket." << std::endl;
            return false;
        }
    #else
        if (m_socket_fd < 0) {
            std::cerr << "Error: Cannot create publisher socket." << std::endl;
            return false;
        }
    #endif

    memset(&m_server_addr, 0, sizeof(m_server_addr));
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, robot_ip.c_str(), &m_server_addr.sin_addr) <= 0) {
        std::cerr << "Error: Invalid IP address." << std::endl;
        return false;
    }

    std::cout << "UDP Publisher initialized. Sending to " << robot_ip << ":" << port << std::endl;
    return true;
}

#ifdef _WIN32
    typedef int ssize_t;
#endif

bool QuestUdpSender::sendJoystickData(float lx, float ly, float rx, float ry) {
    #ifdef _WIN32
        if (m_socket_fd == INVALID_SOCKET) return false;
    #else
        if (m_socket_fd < 0) return false;
    #endif

    JoystickData packet;
    packet.left_x = lx;
    packet.left_y = ly;
    packet.right_x = rx;
    packet.right_y = ry;
    packet.timestamp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    ssize_t bytes_sent = sendto(m_socket_fd, 
                                reinterpret_cast<const char*>(&packet), 
                                sizeof(packet), 
                                0,
                                (const struct sockaddr*)&m_server_addr, 
                                sizeof(m_server_addr));

    return bytes_sent == sizeof(packet);
}