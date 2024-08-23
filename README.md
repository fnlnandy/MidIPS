# MidIPS
`MidIPS` is a mid IPS patcher made in C++ for fun.
The IPS format is "explained" in [FORMAT.md](FORMAT.md), the explanation was provided by [justsolve.archiveteam.org](justsolve.archiveteam.org/wiki/IPS_(binary_patch_format)).

# Usage
The program has two modes that you can pass to it using `-m`, here's the list:
|Argument|Mode|
|--------|----|
|-m=c|Creation of an IPS patch|
|-m=a|Application an IPS patch|

## Creation mode
When in creation mode, those arguments are expected:
- `-c` (mandatory): Specifies the source file.
- `-t` (mandatory): Specifies the target file.
- `-l` (optional): Allows to output the logs in a file instead of to `stdout`.
- `--allow-above-u24` (optional): Allows to override the `0xFFFFFF` limit.

## Application mode
When in application mode, those arguments are expected:
- `-p` (mandatory): Specifies the patch to apply.
- `-a` (mandatory): Specifies the subject file.
- `-l` (optional): Allows to output the logs in a file instead of to `stdout`.
- `--allow-above-u24` (optional): Allows to override the `0xFFFFFF` limit.

# Compiling
Prerequisites:
- On Windows, you would probably download `msys2`.
- A `C++11` capable compiler, just make sure to update the correct [Makefile line](Makefile#L7). Also note that `g++` is used for linking.
- GNU `make`.

Now, if everything is set up correctly, you should able to run:
```shell
$ make -j$(nproc)
```

To clean the projet, e.g. because you've changed a header file, just run:
```shell
$ make clean -j$(nproc)
```
