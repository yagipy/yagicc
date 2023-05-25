# yagicc
yagicc is a tiny C compiler.

```shell
docker build -q .
docker run --rm -it -v $PWD:/yagicc <ID> bash
```

## Build
```shell
make
```

## Test
```shell
make test
```

debug mode
```shell
bash -x test.sh
```
