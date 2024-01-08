# Cloud Goose Storage

Cloud Goose Storage is a file-storage website for Western students to share
files with each other.

## Building

First, make sure all necessary dependencies are installed:

  - A C++ compiler supporting C++20 (except MSVC)
  - CMake
  - Wt, with support for Dbo using the SQLite backend

Then, run these commands to build the project (note that `-B` is **NOT** short
for `--build`; they are different commands):

```sh
cmake -B build
cmake --build build
```

## Running

To run the project, run the following command:

```sh
build/cs3307-group-project --docroot docroot --http-listen '127.0.0.1:8080' -c wt_config.xml
```

The application will start listening for connections on localhost port 8080, as
specified in the command above. The database will be created automatically when
a user first connects.

## Additional notes

### `#pragma once`

`#pragma once` is an alternative to traditional C-style include guards.

> `#pragma once` serves the same purpose as include guards, but with several
> advantages, including less code, avoidance of name clashes, and sometimes
> improvement in compilation speed. ~ [Wikipedia](https://en.wikipedia.org/wiki/Pragma_once)

### Database schema changes

The database schema has changed since Stage 3. If you are running the final
version of this project in the same directory that you previously ran Stage 3
in, you will need to delete `CloudGooseStorage.db` before continuing. Otherwise,
you will encounter many database-related errors while using the program.
