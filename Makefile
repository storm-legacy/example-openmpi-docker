build-image:
	docker compose build

start:
	docker compose up -d

attach:
	docker compose exec controller bash

down:
	docker compose down -v --remove-orphans

logs:
	docker compose logs --tail 100 

logsf:
	docker compose logs --tail 100 -f

stats:
	docker stats

develop: build-image start attach

.SILENT: build-image start down logs logsf stats develop attach