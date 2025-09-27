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

All errors are printed directly to the return value as string, out values as passed as `outParam&`.

### Inspect existing binary bundle data inside a binary

Command: kalaextract inspect all / kalaextract inspect

- confirms if keyword exists
- shows all bundle names and compressed/decompressed size of each bundle
- all params are required

- command: kalaextract inspect x
- inspects the size, data of your chosen bundle by name
- all params are required

### Get existing binary bundle struct data from binary

Command: kalaextract --get x y 

- returns binary bundle by name x from exe y as struct which contains bundle binary, bundle name, bundle index, bundle decompressed size and bundle compressed size
- useful for debugging or getting specific data
- useless in manually run cli because it doesnt print the value, only useful in game or when running in program when you want specific data from the exe itself
- all params are required

### Compress external file/dir into a binary bundle inside target binary

The `--compress` flag is optional, if the target bundle is already marked with the **supported compression extension** and `--compress` is used then the bundle is stored as is.

Command: kalaextract --compress --all x

- compresses all files and folders (except kalaextract itself) in current dir with their name and compressed size and uncompressed size into x exe

Command: kalaextract --compress x y z  

- compresses x path with y name to z exe

Command: kalaextract --compress x y

- compresses x path to y exe

### Decompress existing binary bundle from a binary

The `--decompress` flag is optional, if the target bundle is already not marked with the **supported compression extension** and `--decompress` is used then the bundle is returned as is.

Command: kalaextract --decompress all x y

- decompresses all files from y exe to x path

Command: kalaextract --decompress x y z w

- decompresses x file with y name to z path from w exe

### Replace existing binary bundle from a binary

Command: kalaextract --replace x y z w

- very dangerous if misused because old data is lost forever
- passes x path with y name to w exe
- z is the target bundle that will be replaced, it can either be the original bundle name or index
- all params are required

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