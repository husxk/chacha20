#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define DATA_SIZE 120

// https://datatracker.ietf.org/doc/html/rfc7539

static const uint32_t consts[] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
static const uint32_t key[]    = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
                                   0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c};
static uint32_t counter        =  0x00000001;
static const uint32_t nounce[] = {0x09000000, 0x4a000000, 0x00000000};

static void
update_counter(uint32_t* arr)
{
  memcpy((void*)arr + 48, (void*)&counter, 1 * 4);
  counter++;
}

static void
prepare_matrix(uint32_t* arr)
{
  memcpy((void*)arr     , (void*)consts, 4 * 4);
  memcpy((void*)arr + 16, (void*)key   , 8 * 4);
  memcpy((void*)arr + 52, (void*)nounce, 3 * 4);
}

static void
quarter_round(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d)
{
  *a += *b;
  *d ^= *a;

//  *d <<<= 16;
  asm
  (
    "roll $16, %0"
    : "=r" (*d)
    : "r" (*d)
  );

  *c += *d;
  *b ^= *c;
//  *b <<<= 12;
   asm
  (
    "roll $12, %0"
    : "=r" (*b)
    : "r" (*b)
  );

   *a += *b;
  *d ^= *a;
//  *d <<<= 8;
  asm
  (
    "roll $8, %0"
    : "=r" (*d)
    : "r" (*d)
  );

  *c += *d;
  *b ^= *c;
//  *b <<<= 7;
  asm
  (
    "roll $7, %0"
    : "=r" (*b)
    : "r" (*b)
  );
}

static void
convert(uint32_t* arr)
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
}

static void
print(const void* arr_, const size_t bytes)
{
  const uint8_t* arr = (uint8_t*) arr_;

  printf("\n");
  for(int i = 0; i < bytes; )
  {
    printf("%02x", arr[i++]);
    printf("%02x", arr[i++]);
    printf("%02x", arr[i++]);
    printf("%02x ", arr[i++]);

    if(i % 16 == 0)
      printf("\n");
  }
  printf("\n");
}

static void
add_arrays(uint32_t* arr, uint32_t* arr_conv)
{
  for(int i = 0; i < 16; i++)
    arr_conv[i] += arr[i];
}

static void
set_data(char* data, size_t size)
{
  for(size_t i = 0; i < size; i++)
    data[i] = i;
}

static bool
encrypt(char* data, const size_t data_size, const char* keystream, const size_t keystream_size)
{
  const bool need_keystream = data_size > keystream_size;
  const size_t size = need_keystream  ? keystream_size : data_size;

  for(size_t i = 0; i < size; i++)
    data[i] ^= keystream[i];
  return need_keystream;
}

int
main()
{
  uint32_t arr[16], arr_conv[16];
  char data[DATA_SIZE];

  // init
  set_data(data, DATA_SIZE);
  prepare_matrix(arr);

  printf("\nData:\n");
  print((void*) data, DATA_SIZE);

  bool end;
  size_t data_size = DATA_SIZE;
  size_t skip = 0;

  do
  {
    update_counter(arr);
    memcpy((void*)arr_conv, (void*)arr, 16 * 4);
    convert(arr_conv);
    add_arrays(arr, arr_conv);

    const char* keystream       = (char*)&arr_conv;
    const size_t keystream_size = sizeof(arr_conv);
    end = encrypt(data + skip, data_size, keystream, keystream_size);

    if(end)
    {
      data_size -= keystream_size;
      skip      += keystream_size;
    }

    printf("\narr_conv:\n");
    print((void*) arr_conv, sizeof(arr_conv));
    printf("\ncounter: %08X\n", counter);

  } while(end);

  printf("\nEncrypted data:\n");
  print((void*) data, DATA_SIZE);


}
