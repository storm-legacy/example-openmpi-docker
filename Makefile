include .env

FULL_IMAGE_NAME = $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_TAG)
CONTAINER_TEST_NAME = openmpi-demo-test

build-image:
	docker build -t $(FULL_IMAGE_NAME) .

demo: _start_workers _start_controller down

_start_workers:
	docker compose up -d worker-node

_start_controller:
	docker compose run controller /bin/bash

down:
	docker compose down -v --remove-orphans

logs:
	docker compose logs --tail 100 

logsf:
	docker compose logs --tail 100 -f

.SILENT: build demo _start_workers _start_controller down logs logsf