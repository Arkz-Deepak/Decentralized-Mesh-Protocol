#pragma once
#include "packet_format.h"
#include "routing_engine.h"
#include <stdint.h>
#include "circular_buffer.h"

struct SwarmTask {
    uint16_t task_id;
    uint8_t priority;
    uint16_t assigned_node;
    bool completed;
};

constexpr size_t MAX_TASK_QUEUE_SIZE = 20;

class SwarmOrchestrator {
private:
    uint16_t local_node_id;
    RoutingEngine routing_engine;
    CircularBuffer<SwarmTask, MAX_TASK_QUEUE_SIZE> task_queue;

public:
    explicit SwarmOrchestrator(uint16_t node_id);

    void init();
    void assign_task(uint16_t task_id, uint8_t priority);
    void process_incoming_task(const uint8_t* payload, uint8_t len);
    void execute_orchestration_cycle(uint32_t current_time_ms);
    
    const CircularBuffer<SwarmTask, MAX_TASK_QUEUE_SIZE>& get_tasks() const;
};
