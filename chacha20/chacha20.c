#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "chacha20.h"

// https://datatracker.ietf.org/doc/html/rfc7539

static chacha20_block block =
{
  {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574},   // consts
  {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,    // key
   0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c},
   0x00000000,                                        // counter
  {0x09000000, 0x4a000000, 0x00000000}                // nounce
};

static chacha20_block cipher;

static void
quarter_round(uint32_t* const  a, uint32_t* const b, uint32_t* const c, uint32_t* const d)
{
  *a += *b;
  *d ^= *a;
//  *d <<<= 16;
  asm
  (
    "roll $16, %0"
    : "=rm" (*d)
    : "rm" (*d)
  );

  *c += *d;
  *b ^= *c;
//  *b <<<= 12;
   asm
  (
    "roll $12, %0"
    : "=rm" (*b)
    : "rm" (*b)
  );

  *a += *b;
  *d ^= *a;
//  *d <<<= 8;
  asm
  (
    "roll $8, %0"
    : "=rm" (*d)
    : "rm" (*d)
  );

  *c += *d;
  *b ^= *c;
//  *b <<<= 7;
  asm
  (
    "roll $7, %0"
    : "=rm" (*b)
    : "rm" (*b)
  );
}

static void
convert(uint32_t* arr, const uint32_t* oryginal)
{
  for(int i = 0; i < 10; i++)
  {
    for(int j = 0; j < 4; j++)
      quarter_round(arr + j, arr + j + 4, arr + j + 8, arr + j + 12);

    quarter_round(arr, arr + 5, arr + 10, arr + 15);
    quarter_round(arr + 1, arr + 6, arr + 11, arr + 12);
    quarter_round(arr + 2, arr + 7, arr + 8, arr + 13);
    quarter_round(arr + 3, arr + 4, arr + 9, arr + 14);
  }

  for(int i = 0; i < 16; i++)
    arr[i] += oryginal[i];
}

static void
change_counter()
{
    block.counter++;
}

static void
make_cipher()
{
  memcpy(&cipher, &block, sizeof(chacha20_block));
  convert((uint32_t*)&cipher, (uint32_t*)&block);
}

static void
encrypt_data_block(uint8_t* data, const size_t size)
{
  const char* keystream = (char*)&cipher;
  for(size_t i = 0; i < size; i++)
    data[i] ^= keystream[i];
}

void
decrypt(uint8_t* start, const size_t size, const uint32_t counter)
{
  block.counter = counter;
  make_cipher();
  encrypt_data_block(start, size);
}

uint32_t
encrypt(uint8_t* data, const size_t size)
{
  encrypt_data_block(data, size);

  change_counter();
  make_cipher();
  return block.counter;
}

uint32_t
encrypt_init()
{
  make_cipher();
  return block.counter;
}
