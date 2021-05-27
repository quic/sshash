# Library and tools for hashing sensitive strings
This suite of tools helps protect sensitive information by hashing specific strings in ELF files (binaries, libraries) distributed externally. 
The strings to be hashed must be declared at compile time (see example in tests/basic-test.cc). 
During compilation, the strings are placed in the ELF (Executable and Linkable Format) section ".sshash_str". The resulting binary can be distributed as is internally. 
For the external distribution, any strings in .sshash_str ELF section are replaced with 7 character long alpha-numeric SHAKE128 digest and padded with zeros to 
their original length, so that file size is unchanged. 

## Example Usage
```
cmake .
make
# Important: This step dynamically links against HOGL (https://github.com/maxk-org/hogl). Ensure HOGL is installed before attempting.

# Observe strings that can be scraped from binary
strings -d tests/basic-test

# Hash all strings in .sshash_str section of binary and output hash to string mapping as test.map
tools/sshash-elf --hashmap test.map tests/basic-test

# Observe obfuscated "sensitive information"
strings -d tests/basic-test

# Run binary to generate a logfile with obfuscated sensitive strings
tests/basic-test --format=raw > test.raw.log

# Run txt/xml/json hashing tool
tools/sshash-text --hashmap test.map tests/vects/*.{json,xml,txt}

# View logfile with obfuscated sensitive strings
cat test1.log
# Compare with decoded logfile
cat test1_decoded.log
```

## Advanced User Notes
Helpful debug commands:
```
readelf -p .sshash_str basic-test  # human-readable string dump of .sshash_str section of ELF file basic-test
readelf -x .sshash_str basic-test  # hex dump of same section
```
