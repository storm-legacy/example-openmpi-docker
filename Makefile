# Build universal image
build-image:
	docker compose build

# Start project
start:
	docker compose up -d

# Connect to shell of controller node
attach:
	docker compose exec controller bash

# Stop project and remove all containers (images won't be removed)
down:
	docker compose down -v --remove-orphans

# Look at logs
logs:
	docker compose logs --tail 100 

# Look at logs and follow new ones
logsf:
	docker compose logs --tail 100 -f

# Check resources usage in real time
stats:
	docker stats

# Run sequence of building, starting and attaching to controller container
develop: build-image start attach