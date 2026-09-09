#include <unity.h>
#include <cstring>
#include "packet_format.h"


void setUp(void) {}
void tearDown(void) {}

void test_serial_deserialize(void) {
    MeshPacket original_packet = {};
    original_packet.header.magic = PROTOCOL_MAGIC_BYTE;
    original_packet.header.sender_id = 17;
    original_packet.header.receiver_id = 25;
    original_packet.header.type = static_cast<uint8_t>(PacketType::HEARTBEAT);
    original_packet.header.ttl = 15;
    original_packet.header.payload_len = 25;
    original_packet.header.sequence_num = 115;

    for(int i = 0; i < 25; i++) {
        original_packet.payload[i] = i * 2;
    }

    uint8_t buffer[256];
    size_t out_len = 0;

    bool success = serialize_packet(original_packet, buffer, out_len);
    TEST_ASSERT_TRUE(success);

    MeshPacket reconstructed_packet = {};
    bool deserialize_success = deserialize_packet(buffer, out_len, reconstructed_packet);
    TEST_ASSERT_TRUE(deserialize_success);

    // Test Header fields
    TEST_ASSERT_EQUAL_UINT8(original_packet.header.magic, reconstructed_packet.header.magic);
    TEST_ASSERT_EQUAL_UINT16(original_packet.header.sender_id, reconstructed_packet.header.sender_id);
    TEST_ASSERT_EQUAL_UINT8(original_packet.header.payload_len, reconstructed_packet.header.payload_len);
    
    // Test Payload
    TEST_ASSERT_EQUAL_INT(0, std::memcmp(reconstructed_packet.payload, original_packet.payload, 25));
}

int main(int argc, char **argv) {
    UNITY_BEGIN(); // Start Unity framework
    RUN_TEST(test_serial_deserialize);
    return UNITY_END();
}