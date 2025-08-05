# TODO-C

TODO app made in C because every developer must make one :)

## DEPENDENCIES
```
gnu-make
sqlite3
```

# BUILD INSTRUCTIONS

<<<<<<< HEAD
Made this project on linux, but it also runs on FreeBSD and it should work on any *unix

## Clone the repository
=======
Made this project on linux, but it also runs on FreeBSD and it should work on any bsd

### Clone the repository
>>>>>>> acd2b94 (fix: Makefile and README didn't said enough nor followed standards)

```
    git clone --depth=1 https://github.com/tarffie/todo-c.git
```

<<<<<<< HEAD
## Build && Install using gnu-make

### FreeBSD
```
    gmake && gmake install
```
### Linux
=======
### Build && Install using gnu-make

```FreeBSD
    gmake && gmake install
>>>>>>> acd2b94 (fix: Makefile and README didn't said enough nor followed standards)
```
```Linux
    make && make install
```

## How to use

invoke the app on your terminal like this

```
    todo-c <command> <option>
```

the app supports the following commands

- add <task_name>
- remove <task_id>
- list
