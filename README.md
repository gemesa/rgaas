# rgaas

`rgaas-server` is a service that generates random bytes (using rand() which is not a cryptographically secure generator). The length of the random byte sequence can be specified by the client. The server can handle multiple
random number requests per connection, can handle multiple connections, serves the clients via TCP/IP and can be run as a daemon.

`rgaas-client` is a client to test `rgaas-server` (connect, request and display multiple random byte
sequences of a user-specified length).

## Installation from source

### Prerequisites

The following tools are necessary for building:

- `cmake` (3.23)
- `clang` (14.0.5)

### How to build

Invoke the following commands:

```bash
$ cmake .
$ cmake --build .
```

which will build `rgaas-server` and `rgaas-client` executables. You can find them in the _build_ folder.

## Quickstart

Invoke the following commands (daemon mode):

```bash
$ ./build/rgaas-server -p 8000 -v -d
$ ./build/rgaas-client -n <hostname> -p 8000 -v
```

Note: you will need to send SIGINT to the process of `rgaas-server` to kill it:

```bash
$ kill -INT $(pidof ./build/rgaas-server)
```

Invoke the following commands (foreground mode):

```bash
$ ./build/rgaas-server -p 8000 -v
```
Open an other terminal:

```bash
$ ./build/rgaas-client -n <hostname> -p 8000 -v
```

## For developers

The following tools are necessary to format, check and test your code before `git push`:

- `clang-format` (14.0.5)
- `clang-tidy` (14.0.5)
- `valgrind` (3.19.0)

### How to format, check and test your code

Invoke the following commands to format your code and run static code analysis:

```bash
$ clang-format myfile.c
$ clang-tidy myfile.c
```
or you can recursively format and check all _.c_, _.h_ files in your folder:

```bash
$ find ./myfolder/ -name *.h -o -name *.c | xargs clang-format
$ find ./myfolder/ -name *.h -o -name *.c | xargs clang-tidy
```

Note: `clang-format` uses the _.clang-format_ and `clang-tidy` uses the _.clang-tidy_ config file.
_.clang-format_ has been generated by CLion 2022.2.4 based on personal preferences, _.clang-tidy_ has been generated with the following command:
```bash
$ clang-tidy --warnings-as-errors=* -checks=-*,cert-*,clang-analyzer-*,llvm-*,misc-*,modernize-*,performance-*,portability-*,readability-*,-llvmlibc-restrict-system-libc-headers,-readability-identifier-length,-llvm-header-guard --dump-config > .clang-tidy
```

and is being constantly tailored during the development.

Invoke the following command to run dynamic analysis:

```bash
$ chmod +x ./test/valgrind/valgrind.sh
$ ./test/valgrind/valgrind.sh
```

which will generate the following files in the _build_ folder:

- _valgrind-server.log_
- _valgrind-client.log_
- _rgaas-server.log_
- _rgaas-client.log_

and also print analysis information to stdout.