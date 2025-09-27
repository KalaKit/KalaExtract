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
- all params are required

Command: kalaextract --get x y

- gets the **bundle data** of your chosen bundle by name/index `x` from target binary `y`
- all params are required

### Replace existing binary bundle

Command: kalaextract --replace x y z

- very dangerous if misused because old data is lost forever
- replaces target binary bundle with name/index `x` with new file `y` in target binary `z`
- all params are required

### Compress/store external file/dir

The `--compress` flag is optional, if the target bundle is already marked with the **supported compression extension** and `--compress` is used then the bundle is stored as is.

Command: kalaextract --compress --all x

- compresses all files and folders (except kalaextract itself) in current dir with their name and compressed size and uncompressed size into file `x`

Command: kalaextract --compress x y z  

- compresses path `x` with name `y` inside target binary `z`
- used for when you want a custom name for the bundle

Command: kalaextract --compress x y

- compresses path `x` inside target binary `y`
- used for when you dont want a custom name for the bundle

### Decompress existing binary bundle

The `--decompress` flag is optional, if the target bundle is already not marked with the **supported compression extension** and `--decompress` is used then the bundle is returned as is.

Command: kalaextract --decompress all x y

- decompresses all files to path `x` from target binary `y` 

Command: kalaextract --decompress x y z

- decompresses bundle with name/index `x` to path `y` from target binary `z` 

---

## Data stored in the target binary

These two tables highlight what data is actually stored besides just the bundles, it goes in depth about what each field means and how much space it takes

### Global header data

Offset | Size | Type     | Field                          | Notes
-------|------|----------|--------------------------------|-------------------------------
0–15   | 16   | char[16] | Magic keyword                  | `LOST_EMPIRE_BIN\0` (16 bytes fixed)
16     | 1    | u8       | Bundle count                   | Max 255
17–20  | 4    | u32      | Total compressed size (bytes)  | Sum of all bundles, max ~1.1 TB

### Individual bundle header data

Offset (relative) | Size  | Type     | Field                     | Notes
------------------|-------|----------|---------------------------|------------------------------
0–15              | 16    | char[16] | Magic keyword             | `LOST_EMPIRE_STA\0` (16 bytes fixed)
16                | 1     | u8       | Name length               | Max 64
17–80             | 1–64  | char[N]  | Name data                 | N = name length, must atleast have one char
17+N              | 1     | u8       | Binary index              | 0–255, never reused
18+N              | 4     | u32      | Compressed size (bytes)   | Max ~4.29 GB
22+N              | 4     | u32      | Decompressed size (bytes) | Max ~4.29 GB
26+N              | …     | byte[]   | Compressed bundle data    | Payload
26+N+dataSize     | 16    | char[16] | End magic keyword         | `LOST_EMPIRE_END\0` (16 bytes fixed)