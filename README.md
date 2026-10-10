# NetBSD ls Command - Midterm Project

## Project Information

- Student: Ho Tam Hien
- Student ID: 24IT058
- Project: Reimplementation of the ls command
- Language: C
- Operating System: NetBSD 10.1
- Build tool: make

## Description

This project implements a simplified version of the Unix ls command in C.

The program supports listing files and directories, sorting entries,
displaying file metadata, and recursive directory traversal.

## Supported Options

The program implements 19 command-line options:

| Option | Description |
|--------|-------------|
| -A | Show hidden entries except . and .. |
| -a | Show all entries |
| -c | Use file status change time |
| -d | List directories themselves |
| -F | Append file type indicators |
| -f | Disable sorting |
| -h | Human-readable sizes |
| -i | Show inode numbers |
| -k | Use kilobyte block units |
| -l | Long listing format |
| -n | Numeric user and group IDs |
| -q | Replace non-printable characters |
| -R | Recursive listing |
| -r | Reverse sorting |
| -S | Sort by size |
| -s | Show allocated blocks |
| -t | Sort by time |
| -u | Use access time |
| -w | Display non-printable characters without replacement |

## Project Structure

    Makefile
    README.md
    include/
        ls.h
    src/
        main.c
        options.c
        listing.c
        sorting.c
        format.c
        utils.c

## Build

Run on NetBSD 10.1:

    make

## Usage

    ./myls [options] [file ...]

Examples:

    ./myls
    ./myls -a
    ./myls -l
    ./myls -lh
    ./myls -R src
    ./myls -S src
    ./myls -lsh src

## Testing

The program was tested on NetBSD 10.1 by comparing its output
with the system /bin/ls command.

Recorded results:

- 27/27 option tests passed.
- 12/12 special file tests passed.
- 54/54 regression tests passed.
- 3/3 error-handling tests passed.

The tests covered hidden files, symbolic links, filenames with spaces,
sorting, recursive listing, formatting, and invalid inputs.

These results apply to the tested scenarios.

## Error Handling

The program reports errors for invalid paths and unsupported options.

When valid and invalid paths are provided together, valid files
are still listed and the program returns a nonzero exit status.

## Status

Implementation completed. Tested on NetBSD 10.1.

## Repository

https://github.com/tamhienn/hotamhien_24it058_midterm
