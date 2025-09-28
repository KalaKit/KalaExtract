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

### Compress/store external file/dir

If the target bundle is already marked with the **supported compression extension** then the bundle is stored as is.

Command: kalaextract --compress --all x

- compresses all files and folders (except **KalaExtract** itself) in current dir with their name and compressed size and uncompressed size into file `x`

Command: kalaextract --compress x y z  

- compresses path `x` with name `y` inside target binary `z`
- used for when you want a custom name for the bundle

Command: kalaextract --compress x y

- compresses path `x` inside target binary `y`
- used for when you dont want a custom name for the bundle

### Decompress existing binary bundle

If the target bundle is already not marked with the **supported compression extension** then the bundle is returned as is.

Command: kalaextract --decompress all x y

- decompresses all bundles to path `x` from target binary `y` 

Command: kalaextract --decompress x y z

- decompresses a bundle with name/index `x` to path `y` from target binary `z` 

---

## Data stored in the target binary

These two tables highlight what data is actually stored besides just the bundles, it goes in depth about what each field means and how much space it takes

Some size notes:

- global header size is always `260 bytes`
- each bundle header size is always `784 bytes`
- size of max bundle headers + global header data never exceeds `200KB (200,180 bytes)`
- size of all header sizes + all bundle binary data never exceeds `~1.095 TB`

### Global header data

Offset | Size | Type        | Field         | Notes
-------|------|-------------|---------------|-------------------------------------
0–15   | 16   | char[16]    | Magic keyword | `LOST_EMPIRE_BIN_` (16 bytes fixed)
16–270 | 255  | char[255]   | Bundle IDs    | 254 chars + `\0`
271–274| 4    | u32         | Total size    | Sum of all bundles

### Individual bundle header data

Offset | Size | Type        | Field             | Notes
-------|------|-------------|-------------------|-------------------------------------
0–15   | 16   | char[16]    | Magic keyword     | `LOST_EMPIRE_STA_` (16 bytes fixed)
16–270 | 255  | char[255]   | Bundle name       | 254 chars + `\0`
271–525| 255  | char[255]   | Bundle index      | Fixed, zero-padded text index
526–529| 4    | u32         | Compressed size   | Bundle compressed size before adding to target binary
530–533| 4    | u32         | Decompressed size | Bundle decompressed size before adding to target binary
534–788| 255  | char[255]   | Origin path       | 254 chars + `\0`
789–792| 4    | u32         | pos_headerEnd     | Distance from bundle header end to bundle end magic
793–796| 4    | u32         | pos_endMagicStart | Distance back from bundle end magic to bundle header end
797–812| 16   | char[16]    | End magic keyword | `LOST_EMPIRE_END_` (16 bytes fixed)