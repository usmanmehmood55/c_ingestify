# Ingestify C

This is an implementation of Ingestify in C, for practicing C.

## What's Ingestify?

When I'm using ChatGPT and I have to provide it a codebase, I have noticed that
simply uploading the code files has no effect (as of now). Instead I always have
to put the code in the chat, one file at a time.

So I made a small tool in .NET which takes a codebase and writes all the files
into a single `.txt` file for easy copy pasting. More importantly it has a Git-like
ignore feature which prevents things like build files and images etc to not be
included in the output text file.

Put simply, the requirements are:

1. Takes in a folder path of a codebase, and reads all the files, and files in subfolders.
2. Writes the entire codebase into a single text file.
3. Writes the relative file path before writing the file contents.
4. Has Git-like ignore functionality so user can ignore non-code files like build artifacts, images, etc.

The .NET version can be found here: [Ingestify](https://github.com/usmanmehmood55/Ingestify)

And while .NET provides exceptionally easy ways to manage files, paths, strings etc,
and also has a library that implements all of the git ignore functionality,
implementing the same in C is very difficult (for someone like me).

And so I'm using this as a way to practice and learn C.

## Setup

Use my VSCode extension, [C Toolkit](https://marketplace.visualstudio.com/items?itemName=UsmanMehmood.c-toolkit)
for compiling and running.

And add the [C Asserts](https://github.com/usmanmehmood55/c_asserts) git submodule.

## Use

The app takes two or three CLI inputs.

```bash
c_ingestify.exe <input folder> <output file> <ignore file, optional>
```

For example:

```bash
c_ingestify.exe MyAwesomeApp output.txt ingestify_ignore.txt
```

## Ongoing Issues

- The ignore functionality DOES NOT fully work.
- It can do cases like
  - `file.type`
  - `path`
  - `folder/`
  - `folder/file.type`
  - `*.type`
  - `**/folder`
  - `**/folder/file.type`
  - `!file.type`
  - `file?.type`
- It cannot do cases like
  - `file[num].type`
  - `file[num_range].type`
  - `file[!num_range].type`
  - `file[letter_range].type`
  - `folder/**/file.type`
  - `folder/*folder/file.type`
- Tests for all the types have been written.

I am basing the criteria from this .gitignore guide from Atlassian: [Git ignore patterns](https://www.atlassian.com/git/tutorials/saving-changes/gitignore).
