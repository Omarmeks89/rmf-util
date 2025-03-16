### ABOUT
`rmf` is a small util to avoid unexpected file deleting (`rm -rf` command).
This moment is possible to store new files only.

### Installation
- Clone repository in wished directory
```
git clone git@github.com:Omarmeks89/rmf-util.git
```
- run `make install` command
```
make install
```

`rmf` installer create working directory by `$HOME/.rmf` path.
All links will bee stored there

### Using

Save wished file to avoid unexpected deleting
```
./rmf text.txt
```

