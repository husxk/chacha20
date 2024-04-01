#pragma once

typedef struct __attribute__((packed))
{
  uint32_t counter;
  uint8_t data[64];
} counter_packet;
