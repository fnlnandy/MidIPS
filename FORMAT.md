1. Header
The file starts with a "magic header":
`50 41 54 43 48`

2. Data
The data's structure is pretty simple:
* Offset  = `0x3` bytes
* Length  = `0x2` bytes
* Count   = `0x2` bytes\*
* Byte(s) = `0x1` \* `Length` bytes *or* `0x1`\*\* byte

\*: Is only read if `Length` is set to 0.

\*\*: Only `0x1` byte is expected if `Length` is `0`, which will serve as the filler.
