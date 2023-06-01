# OpenMPI Example with Docker
### Only for educational/development purposes
Although the project could not be considered malicious in any way, there are a few elements that discredit this configuration in a production environment. Mainly:

*   Containerization of the C++ application might be beneficial in terms of distributed calculations, but this runs solely on one machine, which makes this setup overkill for most applications.
*   The used Docker image is Debian-based, which might not be the best choice considering Alpine alternatives. Additionally, all tools used to build MPI libraries are still present, which significantly increases image size.
*   The usage of generated SSH keys shared between all containers with the same version of the image is at least insecure.

This readme does not focus on OpenMPI but on the technologies used to run an application built with this library.

### Implemented Algorithm
Pi estimation method with Monte Carlo method.

## Dependencies
*   Docker Engine + CLI (e.g., Docker Desktop)
*   Docker Compose Plugin
*   make

## Running project
Build the image with:
```bash
$ make build-image
```

Run the project with:
```bash
$ make start
```

Connect to the controller container:
```bash
$ make attach
```

Run the application (from within the _controller_ container):
```bash
$ ./start.sh
```

Control resource usage (run on the host):
```bash
$ docker stats
```

Clean after testing (run on host):
```
$ make down
```

## Customization
There are two major values possible to adjust:
*   `SAMPLES_POOL` - the number of generated points (x,y) that will be shared across nodes.
*   `WORKERS_NUM` - the number of spawned processes, doesn't necessarily need to be equal to slots in `hostfile.txt`.

### Application
Before running the script, the configuration can be adjusted either via:

```bash
$ export WORKERS_NUM=4
$ export SAMPLES_POOL=1000
$ ./start.sh
```
or
```bash
$ WORKERS_NUM=4 SAMPLES_POOL=1000 ./start.sh
```

Additionally, values can be adjusted before starting in `docker-compose.yml` at the path: `services.controller.environment`.

  ```yml
  (...)
    environment:
      WORKERS_NUM: 9
      SAMPLES_POOL: 100000000
  ```

### Start script (start.sh)
When further changes are needed for the starting command, those values can be applied to `/app/start.sh` script, especially to this line:

```bash
/usr/local/bin/mpirun -n ${WORKERS_NUM} --hostfile hostfile.txt /app/application ${SAMPLES_POOL}
```

Keep in mind that `/usr/local/bin/mpirun` must be specified in this manner because specifying just `mpirun` could generate problems while trying to connect to remote nodes. The same goes for `/app/application`.

### Limits reduction
Each node has a limit of one cpu, which is specified in `docker-compose.yml` under path: `services.node-x.cpus`. It allows for testing the performance with workers number incrementation.
```yml
  node-1:
    (...)
    cpus: 1
```

## Insight
### General
The application is tasked with calculating the approximation of Pi using the Monte Carlo algorithm with multiple nodes running as containers. Nodes are interconnected via internal Docker networking. Information is exchanged via the SSH protocol.

### Image
The built Docker image is shared among all the nodes because, for remote computing, the `mpirun` command must be present on the external host. The layers that together make up the image are:

*   **System update step** - updating the packages and installing dependencies for the next steps. These packages include tools for building applications, certificates for HTTPS connection, SSH server, and doas (an alternative for sudo).
*   **OpenMPI build step** - here, the process of building OpenMPI from the source and installing it into the machine takes place.
*   **Application build step** - here, the process of building our application takes place using tools and libraries from earlier steps.
*   **System prep step** - configuration of the SSH server is done here, a user for privileges drop is created, and (for the sake of convenience, although insecure) the same user gets sudo permissions.

By default, the image's starting command (which every container must have) is an SSH server running in the foreground (which allows for collecting server logs).

### Permissions drop
The image has a user named `app` who runs the application. This setting allows skipping the argument `--allow-run-root` of `mpirun`. It might be a little overkill, but it still shows a way to approach security in containerized environments. In production, this user mustn't have `sudo` permissions, but in this case, it allows for fast fixes on a running container.

### Workers
Workers use the default configuration and listen on the SSH port for active connections. SSH servers are running as the `app:app` user, which results in denied permission to modify system files.

### Controller
This service is running indefinitely without services to be able to attach to it. From this place, jobs can be distributed to workers, but it is not impossible to do it from any other worker with some configuration tinkering.

### SSH Server
Connection is done via SSH on port 22 with public key authentication. Doing it this way allows for a safe connection flow without the usage of a password that would need to be stored somewhere. Within `/app/start.sh`, the SSH block is in charge of getting the keys needed for SSH authentication with every server on `hostfile.txt`. This step is required because of the non-interactivity of the OpenMPI protocol.

### CMake
The tool used for building the C++ application in this project is called CMake and is widely used for all kinds of projects. It's an industry standard and allows for management of dependencies. CMake links external libraries to the project, which is extremely useful in the case of building OpenMPI applications.

### Makefile
In this project, the Makefile is used as a tool for simplification of more complex commands. It allows simplification for groups of commands as an alternative to user-defined scripts.

## Useful commands
For a list of the useful commands, open the `Makefile` file in the repository.