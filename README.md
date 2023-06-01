# OpenMPI Example with Docker

## Information
Used Monte Carlo Algorithm to calculate aproximation of pi.

## Dependencies
- Docker Engine + CLI (ex. Docker Desktop)
- make

## Running project
Build image with:
```
$ make build-image
```

Run project with:
```
$ make start
```

Connect to controller container:
```
$ make attach
```

Run application with (from within *controller* container)
```
$ ./start.sh
```

Controll resources usage with (run on host):
```
$ docker stats
```

## More information
For list of the useful commends open `Makefile` file in repository.
