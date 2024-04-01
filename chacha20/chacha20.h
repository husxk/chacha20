#pragma once
#include <stdbool.h>

typedef struct
{
  const uint32_t consts[4];
  const uint32_t key[8];
        uint32_t counter;
  const uint32_t nounce[3];
} chacha20_block;

#define DATA_SIZE sizeof(chacha20_block)

uint32_t
encrypt_init();

uint32_t
encrypt(uint8_t*, const size_t);

void
decrypt(uint8_t*, const size_t, const uint32_t);
