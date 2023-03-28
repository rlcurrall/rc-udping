# RC UDPing

This was just an afternoon experiment to better understand how communication
between threads works in C using signals and a shared memory queue.

---

## Build

To build the project, you need to have `gcc` installed.

```bash
gcc -pthread -o udping main.c queue.c ping.c listen.c
```

The project can also be built using the provided `Makefile` which requires
`make` to be installed. You can build the project using:

```bash
make
```

## Usage

Start the listener:

```bash
./udping listen
```

Start the pinger:

```bash
./udping ping
```
