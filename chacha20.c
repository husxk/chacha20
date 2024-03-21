#include <stdio.h>
#include <stdint.h>
#include <string.h>

// https://datatracker.ietf.org/doc/html/rfc7539

static const uint32_t consts[] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
static const uint32_t key[]    = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
                                   0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c};
static const uint32_t block    =  0x00000001;
static const uint32_t nounce[] = {0x09000000, 0x4a000000, 0x00000000};

static void
prepare_matrix(uint32_t* arr)
{
  memcpy((void*)arr     , (void*)consts, 4 * 4);
  memcpy((void*)arr + 16, (void*)key   , 8 * 4);
  memcpy((void*)arr + 48, (void*)&block, 1 * 4);
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
  uint32_t arr_origin[16];
  memcpy((void*)arr_origin, (void*)arr, 16);

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
print(uint32_t* arr)
{
  printf("\n");
  for(int i = 0; i < 16; i++)
  {
    printf("%08x  ", arr[i]);

    if((i + 1) % 4 == 0)
      printf("\n");
  }
}

static void
add_arrays(uint32_t* arr, uint32_t* arr_conv)
{
  for(int i = 0; i < 16; i++)
    arr_conv[i] += arr[i];
}

int
main()
{
  uint32_t arr[16], arr_conv[16];
  prepare_matrix(arr);
  print(arr);

  memcpy((void*)arr_conv, (void*)arr, 16 * 4);
  convert(arr_conv);
  print(arr_conv);

  add_arrays(arr, arr_conv);
  print(arr_conv);
}
