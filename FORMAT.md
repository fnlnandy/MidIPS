## Header
The patcher expects the IPS file to start with the common header:
```
50 41 54 43 48
```
Which translates to `"PATCH"`, apparently.

## Data format
The data is laid out like so:
- The first `0x3` bytes are designing the offset of insertion, which is 24 bits, making it impossible to apply changes past a 16MB file.
- The second `0x2` bytes are designing the length of the data, the byte array. If they're set to `0`, the next `0x2` bytes are interpreted as a count (for filling).
- The last `0x1 * Length` bytes represent the raw data to write, if `Length` is `0`, then only the first byte is read to fill `0x1 * Count` bytes.
