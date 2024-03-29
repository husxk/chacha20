import sys
import random

if len(sys.argv) != 3:
    print("Usage: ./" + sys.argv[0] + " <file_to_write> <number_of_bytes")
    print("<number_of_bytes> must be < 256")
    print("Example: ./" + sys.argv[0] + " super_nice_file 123");
    exit()

number = int(sys.argv[2])
if number > 255:
    print("<number_of_bytes> must be < 256)")
    exit()

rand = random.sample(range(255), number)

with open(sys.argv[1], "wb") as file:
    for i in rand:
        file.write(i.to_bytes(1, byteorder='little'))
