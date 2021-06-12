# qemu-copy-tools

This is a simple tool for transferring files between a qemu host and guest.

## What runs where?

* Host - The computer running qemu.
* Guest - The environment running under qemu.

* qh - Runs on the host. qg connects to this. Start this first.
* qg - Runs on the guest. qg connects to qh. Start this second.

## Transferring a file from host to guest

On host:

```
$ qh file.txt
qh: Waiting for connection.
```

On guest:

```
$ qg
```

Once the file is finished transferring, both programs will quit.

## Transferring a file from guest to host

On host:

```
$ qh
qh: Waiting for connection.
```

On guest:

```
$ qg file.txt
```

Once the file is finished transferring, both programs will quit.

## How it works

qh opens a TCP socket on port 12345. Once open, it will accept a connection. An argument present when calling either qh or qg will put that program into send mode, while a lack of an argument puts the particular program into receive mode.

Once the client socket is connected, the sending program sends 256 bytes containing a filename, which is received. The receiver opens the file for writing, and takes data from the input into the resulting file until the socket is closed by the sender.

Because the server is always the host, and on a known address and port, it will work out of the box with the default NAT (SLIRP) networking provided by qemu.


