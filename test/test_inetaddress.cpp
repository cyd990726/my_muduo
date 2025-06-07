#include <gtest/gtest.h>
#include "../InetAddress.hpp"




TEST(InetAddressTest, toIp){
    InetAddress addr(1999, "127.0.0.1");
    EXPECT_EQ(addr.toIp(), "127.0.0.1");
}

TEST(InetAddressTest, toIpPort){
    InetAddress addr(1999, "127.0.0.1");
    EXPECT_EQ(addr.toIpPort(), "127.0.0.1:1999");
}

TEST(InetAddressTest, toPort){
    InetAddress addr(1999, "127.0.0.1");
    EXPECT_EQ(addr.toPort(), 1999);
}

TEST(InetAddressTest, GetSockAddr) {
    InetAddress addr(8080, "192.168.1.1");

    const sockaddr_in* p = addr.getSockAddr();
    // 检查地址族
    EXPECT_EQ(p->sin_family, AF_INET);
    // 检查端口（注意是网络字节序）
    EXPECT_EQ(ntohs(p->sin_port), 8080);
    // 检查IP
    char buf[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &(p->sin_addr), buf, sizeof(buf));
    EXPECT_EQ(std::string(buf), "192.168.1.1");
}

TEST(InetAddressTest, SetSockAddr) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    inet_pton(AF_INET, "10.0.0.1", &addr.sin_addr);

    InetAddress inetAddr;
    inetAddr.setSockAddr(addr);

    // 检查 getSockAddr 返回的内容
    const sockaddr_in* p = inetAddr.getSockAddr();
    EXPECT_EQ(p->sin_family, AF_INET);
    EXPECT_EQ(ntohs(p->sin_port), 12345);

    char buf[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &(p->sin_addr), buf, sizeof(buf));
    EXPECT_EQ(std::string(buf), "10.0.0.1");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}