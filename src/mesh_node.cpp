#include "mesh_node.h"
#include <cstring>
#include <algorithm>
#include <iostream>

MeshNode::MeshNode(uint16_t id) : node_id(id), current_seq(0) {}

void MeshNode::init() {
    routing_table.clear();
    seen_packets.clear();
}

bool MeshNode::is_duplicate(uint16_t seq) {
    if (std::find(seen_packets.begin(), seen_packets.end(), seq) != seen_packets.end()) {
        return true;
    }
    if (seen_packets.size() > 50) {
        seen_packets.erase(seen_packets.begin());
    }
    seen_packets.push_back(seq);
    return false;
}

void MeshNode::update_peer(uint16_t sender_id, int8_t rssi, uint8_t hops) {
    PeerInfo& peer = routing_table[sender_id];
    peer.node_id = sender_id;
    peer.rssi = rssi;
    peer.hop_count = hops;
}

void MeshNode::handle_received_packet(const uint8_t* raw_data, size_t len, int8_t rssi) {
    MeshPacket packet;
    // Step 1: Validate framing, magic byte, and CRC16 checksum
    if (!deserialize_packet(raw_data, len, packet)) {
        return;
    }

    // Step 2: Drop packets originated by this node to prevent self-echo loops
    if (packet.header.sender_id == node_id) {
        return;
    }

    // Step 3: Check cache for duplicate packets to avoid redundant processing
    if (is_duplicate(packet.header.sequence_num)) {
        return;
    }

    // Step 4: Update neighbor routing table with latest RSSI and observed hop count
    update_peer(packet.header.sender_id, rssi, packet.header.ttl);

    // Step 5: Process payload locally if packet is addressed to this node or is a broadcast
    if (packet.header.receiver_id == node_id || packet.header.receiver_id == 0xFFFF) {
        // Core payload processing hook for swarm intelligence
    }

    // Step 6: Multi-hop relay logic (Issue #13)
    // Forward packet if it is a broadcast or destined for another node in the mesh
    if (packet.header.receiver_id != node_id) {
        // Drop packet immediately if TTL has expired to prevent infinite broadcast storms
        if (packet.header.ttl <= 1) {
            std::cout << "[MESH WARN] Packet dropped! TTL expired." << std::endl;
            return;
        }

        // Decrement hop count for intermediate relay
        packet.header.ttl -= 1;

        // Re-serialize packet with updated TTL and forward to next hop
        uint8_t forward_buffer[256];
        size_t forward_len = 0;
        if (serialize_packet(packet, forward_buffer, forward_len)) {
            std::cout << "[MESH INFO] Relaying Packet. New TTL: " << static_cast<int>(packet.header.ttl) << std::endl;
        }
    }
}

bool MeshNode::broadcast_payload(PacketType type, const uint8_t* data, uint8_t len) {
    if (len > MAX_PAYLOAD_SIZE) return false;

    MeshPacket packet;
    packet.header.magic = PROTOCOL_MAGIC_BYTE;
    packet.header.type = static_cast<uint8_t>(type);
    packet.header.sender_id = node_id;
    packet.header.receiver_id = 0xFFFF;
    packet.header.sequence_num = ++current_seq;
    packet.header.ttl = 10;
    packet.header.payload_len = len;

    if (data && len > 0) {
        std::memcpy(packet.payload, data, len);
    }

    return true;
}

bool MeshNode::send_to_node(uint16_t target_id, PacketType type, const uint8_t* data, uint8_t len) {
    if (len > MAX_PAYLOAD_SIZE) return false;

    MeshPacket packet;
    packet.header.magic = PROTOCOL_MAGIC_BYTE;
    packet.header.type = static_cast<uint8_t>(type);
    packet.header.sender_id = node_id;
    packet.header.receiver_id = target_id;
    packet.header.sequence_num = ++current_seq;
    packet.header.ttl = 10;
    packet.header.payload_len = len;

    if (data && len > 0) {
        std::memcpy(packet.payload, data, len);
    }

    return true;
}

void MeshNode::cleanup_dead_peers(uint32_t timeout_ms, uint32_t current_time_ms) {
    for (auto it = routing_table.begin(); it != routing_table.end();) {
        if (current_time_ms - it->second.last_seen_ms > timeout_ms) {
            it = routing_table.erase(it);
        } else {
            ++it;
        }
    }
}

const std::unordered_map<uint16_t, PeerInfo>& MeshNode::get_routing_table() const {
    return routing_table;
}
