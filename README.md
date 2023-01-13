# libexpress

Nodemon for C :) It is speed.

## Dependencies

Following need to be installed first:
- [libcol](https://github.com/resyfer/libcol)
- [libtpool](https://github.com/resyfer/libtpool)
- [libhmap](https://github.com/resyfer/libhmap)
- [libqueue](https://github.com/resyfer/libqueue)
- [libvector](https://github.com/resyfer/libvector)

## Install

```sh
git clone https://github.com/resyfer/cdemon.git
cd cdemon
make install
cd ..
rm -rf cdemon
```

## Usage

If you want to run the command:
```sh
ls -alh
```

just add a `cdemon` before it:
```sh
cdemon ls -alh
```