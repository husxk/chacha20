# chacha20
It is simple server-client communication over UDP with encryption using chacha20.
Server sends bytes from `data.txt` to client.

## Compilation
To compile, in the main directory run

`$ make`


## Usage
In ***separate*** terminals run

`$ make rserver`

`$ make rclient`

# create_file.py
Generates binary file with up to 255 random bytes
## Usage
`$ python3 create_file.py <filename> <number_of_bytes>`

For example:

`$ python3 create_file.py data.txt 123`

