# simple-strace
Created to learn how strace(1) works.

## Usage
Build the program with the following command.
```sh
make
```

### When tracing a newly executed program
```sh
./strace PROG [ARGS]
```
For example, to trace `ls -l`
```sh
./strace ls -l
```

### When tracing a program that is already running
```sh
./strace -p PID
```
For example, to trace a running top command (pid = 27891')
```sh
./strace -p 27891
```