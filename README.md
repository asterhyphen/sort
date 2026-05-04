# File Sorting Program
A simple program to rename your files numerically on your computer based on ` date_creation `.
- To run it, just save the files of your language of choice to the folder where you want the files to be renamed, and run! (compile if required)

## Syntax
### C
```bash
gcc c_version.c
./a.out '<folderpath_1>' '<folderpath_2>' '<folderpath_n>'
```

### Python
```bash
python3 python_version.py <folderpath_1> <folderpath_2> <folderpath_n>
```

### Rust
```bash
rustc rust_version.rs
./rust_version.rs <folderpath_1> <folderpath_2> <folderpath_n>

or
cargo run -- <folderpath_1> <folderpath_2> <folderpath_n>
```
> Do not use <> in your actual command, they indicate replacement to be done to the command

## Navigate to src files
- *[Rust](src/rust_version.rs)*
- *[C](src/c_version.c)*
- *[Python](src/python_version.py)* </br>

> [!NOTE]
> Ensure that you have prerequisites like Python (for Python version), C compiler (for C), or Rust installed (for Rust).
