### ABOUT
`rmf` is a small util used to avoid unexpected file deleting.
It not used a lot of additional memory, only needed to save
new `inode` pointers.

#### Installation

1) Compile executable file:
```bash
./build.sh
```

2) Create user and group (as sudo):
```bash
./install.sh
```

#### Uninstall

Run `./uninstall.sh` script. It will remove all created ELF files, users and groups:
```bash
./uninstall.sh
```
