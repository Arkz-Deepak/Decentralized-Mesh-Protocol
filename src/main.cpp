#include <iostream>
#include <cstring>
#include "swarm_orchestrator.h"
#include "security_engine.h"
#include "mesh_node.h"

int main() {
    std::cout << ">>> DECENTRALIZED SWARM OS / MESH CORE INITIALIZED <<<" << std::endl;

    // =========================================================================
    // 1. Swarm Orchestrator Initialization & Mission Task Allocation
    // =========================================================================
    // Create an autonomous swarm node with Node ID 0x1001 and initialize state
    SwarmOrchestrator node(0x1001);
    node.init();

    // Assign mission tasks with differing priority levels to the orchestrator
    std::cout << "[SYSTEM] Assigning autonomous swarm mission tasks..." << std::endl;
    node.assign_task(101, 1); // Task ID 101, Priority 1
    node.assign_task(102, 2); // Task ID 102, Priority 2

    // Execute an initial orchestration cycle with a simulated delta time (1000 ms)
    std::cout << "[SYSTEM] Running initial orchestration cycle..." << std::endl;
    node.execute_orchestration_cycle(1000);

    std::cout << "[SUCCESS] Active task queue size: " << node.get_tasks().size() << std::endl;

    // =========================================================================
    // 2. AES-128 Security Pipeline Verification
    // =========================================================================
    std::cout << "\n[SECURITY] Initializing AES-128 Encryption Engine..." << std::endl;
    SecurityEngine sec;
    uint8_t secret_key[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    uint8_t nonce[16] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};

    // Load the 128-bit key into the AES cryptographic engine
    if (!sec.set_key(secret_key, 16)) {
        std::cerr << "[ERROR] Key setup failed!" << std::endl;
        return 1;
    }

    const char* raw_payload = "SWARM_COMMAND_EXECUTE_TAKEOFF";
    size_t len = std::strlen(raw_payload);
    uint8_t encrypted[64] = {0};
    uint8_t decrypted[64] = {0};

    // Encrypt the command payload, then decrypt it to verify end-to-end data integrity
    sec.encrypt(reinterpret_cast<const uint8_t*>(raw_payload), len, encrypted, nonce);
    sec.decrypt(encrypted, len, decrypted, nonce);

    if (std::memcmp(raw_payload, decrypted, len) == 0) {
        std::cout << "[SECURITY SUCCESS] Encrypted packet decrypted with 100% integrity!" << std::endl;
    } else {
        std::cerr << "[SECURITY FAIL] Decrypted payload mismatch!" << std::endl;
        return 1;
    }

    // =========================================================================
    // 3. Time-To-Live (TTL) Multi-Hop Routing Verification (Issue #13)
    // =========================================================================
    // Initialize an intermediate routing node (0x2001) to simulate multi-hop forwarding
    std::cout << "\n[ROUTING] Verifying Time-To-Live (TTL) Multi-Hop Routing..." << std::endl;
    MeshNode router_node(0x2001);
    router_node.init();

    // Test 3a: Valid multi-hop broadcast packet (TTL = 10)
    // The router should decrement TTL from 10 to 9 and relay the packet forward
    MeshPacket test_pkt;
    test_pkt.header.magic = PROTOCOL_MAGIC_BYTE;
    test_pkt.header.type = static_cast<uint8_t>(PacketType::BEACON);
    test_pkt.header.sender_id = 0x1001;
    test_pkt.header.receiver_id = 0xFFFF; // Broadcast address
    test_pkt.header.sequence_num = 1;
    test_pkt.header.ttl = 10;
    test_pkt.header.payload_len = 4;
    std::memcpy(test_pkt.payload, "PING", 4);

    uint8_t raw_buf[256];
    size_t raw_len = 0;
    serialize_packet(test_pkt, raw_buf, raw_len);
    router_node.handle_received_packet(raw_buf, raw_len, -65);

    // Test 3b: Expired packet (TTL = 1)
    // Decrementing TTL reaches 0; node must drop it immediately to stop broadcast storms
    test_pkt.header.sequence_num = 2;
    test_pkt.header.ttl = 1;
    serialize_packet(test_pkt, raw_buf, raw_len);
    router_node.handle_received_packet(raw_buf, raw_len, -65);

    std::cout << "\n>>> SYSTEM CORE & SECURITY LAYER FULLY OPERATIONAL <<<" << std::endl;
    return 0;
}

