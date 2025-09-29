# KalaExtract 1.0 spec sheet

The purpose of this document is to highlight all possible ways you can use to pass data to any binary with a few simple but powerful commands that can be ran outside of your exe from the console of your OS or directly inside an exe with the system command or other ways you prefer to do it.

**KalaExtract** itself bundles with **7zip** binaries baked into its exe so **7zip** is not required to be downloaded by the end user.

You can pass binary bundles to ANY file that allows writing to it and won't break if you try to get data from it. This is risky if you write to a custom binary with required size or if this binary is modified somewhere else, but **KalaExtract** doesnt care, exe is just the most useful type of binary data to use **KalaExtract** with.

## Supported compression extensions

These extensions were chosen because they're OS-agnostic since KalaExtract aims for Windows and Linux.

- .7z
- .zip
- .tar
- .tar.gz
- .tar.xz

---

## Commands

All errors are printed directly to the console output as string.

Supported flags at the end of commands (only one of the exit variants at one time):
- --exit - call command and require user to press `enter` to exit
- --quickexit - call command and immediately exit afterwards
- --nosplash - does not re-display the splash screen every time a new command is inserted (quirk of the cli) 

### Get info about commands

Command: --help

- lists all available commands

Command: --help x

- lists detailed info about command `x`

### Exit KalaExtract

Command: --exit

- Closes **KalaExtract** but requires you to press `enter` to exit so you can still read logs if needed

Command: --quickexit

- Closes **KalaExtract** and exits straight away with no further input needed

### Create a new target binary

The purpose of this command is to let you create any binary file at any path so you can pass data to it with **KalaExtract**.

Command: kalaextract --create x

- create a basic binary at path `x`
- useful for creating empty binaries you can pass bundles directly into

### Get existing binary bundle data

Printed bundle data:
- name
- index
- decompressed size
- compressed size
- the byte where the bundle data starts
- the byte where the bundle data ends

Command: kalaextract --get --all x

- gets all **bundle data** of each bundle from target binary `x`

Command: kalaextract --get x y

- gets the **bundle data** of your chosen bundle by name/index `x` from target binary `y`

### Replace existing binary bundle

Replaced original bundle data is lost forever, it is recommended to decompress it first if you wanna keep it

Command: kalaextract --replace x y z

- replaces target binary bundle with name/index `x` with new file `y` in target binary `z`

### Remove existing binary bundle

Replaced bundle data is lost forever, it is recommended to decompress it first if you wanna keep it

Command: kalaextract --remove --all x

- removes all bundles from target binary `x`

Command: kalaextract --remove x y

- removes target binary bundle with name/index `x` from target binary `y`

### Reset target binary

Removed bundle data is lost forever, it is recommended to decompress it first if you wanna keep it

Command: kalaextract --reset x

- removes all bundles from target binary `x` and the global header, as if no external data was never passed to the target binary
- this command does not "reset" to original file size, instead it removes the header and all bundles and their headers within the KalaExtract range, this preserves data placed by another executable after any KalaExtract data

### Pack external file/dir

If the optional `--compress` flag is used and the target file or directory is already marked with the **supported compression extension** then the bundle is stored as is.

Command: kalaextract --pack --compress --all x

- packs and compresses all files and folders (except **KalaExtract** itself) in current dir with their name and compressed size and uncompressed size into file `x`

Command: kalaextract --pack --compress x y z  

- packs and compresses path `x` with name `y` inside target binary `z`
- used for when you want a custom name for the bundle

Command: kalaextract --pack --compress x y

- packs and compresses path `x` inside target binary `y`
- used for when you dont want a custom name for the bundle

### Decompress existing binary bundle

If the optional `--decompress` flag is used and the target file or directory is already not marked with the **supported compression extension** then the bundle is returned as is.

Command: kalaextract --unpack --decompress all x y

- unpacks and decompresses all bundles to path `x` from target binary `y` 

Command: kalaextract --unpack --decompress x y z

- unpacks and decompresses a bundle with name/index `x` to path `y` from target binary `z` 

---

## Data stored in the target binary

These two tables highlight what data is actually stored besides just the bundles, it goes in depth about what each field means and how much space it takes

Some size notes:

- global header size is always `276 bytes`
- each bundle header size is always `816 bytes`
- size of max bundle headers + global header data never exceeds `~203.5 KB (208,356 bytes)`
- size of all header sizes + all bundle binary data never exceeds `~1.095 TB`

### Global header data

Offset | Size | Type        | Field         | Notes
-------|------|-------------|---------------|-------------------------------------
0–15   | 16   | char[16]    | Magic keyword | `LOST_EMPIRE_BIN` (16 bytes fixed)
16–271 | 256  | char[256]   | Bundle IDs    | 255 chars + `\0`
272–275| 4    | u32         | Total size    | Sum of all bundles

### Individual bundle header data

Offset | Size | Type        | Field             | Notes
-------|------|-------------|-------------------|-------------------------------------
0–15   | 16   | char[16]    | Magic keyword     | `LOST_EMPIRE_STA` (16 bytes fixed)
16–271 | 256  | char[256]   | Bundle name       | 255 chars + `\0`
272–527| 256  | char[256]   | Bundle index      | Fixed, zero-padded text index
528–531| 4    | u32         | Compressed size   | Bundle compressed size before adding to target binary
532–535| 4    | u32         | Decompressed size | Bundle decompressed size before adding to target binary
536–791| 256  | char[256]   | Origin path       | 255 chars + `\0`
792–795| 4    | u32         | pos_headerEnd     | Distance from bundle header end to bundle end magic
796–799| 4    | u32         | pos_endMagicStart | Distance back from bundle end magic to bundle header end
800–815| 16   | char[16]    | End magic keyword | `LOST_EMPIRE_END` (16 bytes fixed)
