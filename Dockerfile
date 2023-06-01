FROM debian:bullseye-slim AS base
ARG OPENMPI_URL="https://download.open-mpi.org/release/open-mpi/v4.1/openmpi-4.1.5.tar.gz"

# System update step
RUN set -x \
  && export DEBIAN_FRONTEND=noninteractive \
  && apt-get update \
  && apt-get install --no-install-recommends -y \
  build-essential \
  ca-certificates \
  dropbear \
  openssh-client \
  cmake \
  wget \
  doas vim nano 

# OpenMPI build step
RUN set -x \
  && wget -q ${OPENMPI_URL} -O /tmp/openmpi.tar.gz \
  && tar -xvzf /tmp/openmpi.tar.gz -C /tmp/ \
  && cd /tmp/openmpi* \
  && ./configure \
  && make -j$(($(nproc --all) - 1)) \
  && make install \
  && ldconfig \
  && rm -rf /tmp/openmpi*

WORKDIR /app

# Application build step
COPY [".", "/tmp/"]
RUN set -x \
  && cmake -S /tmp -B /tmp/build \
  && cmake --build /tmp/build \
  && cp /tmp/build/bin/* /app/ \
  && rm -rf /tmp/*

ENV WORKERS_NUM=2 \
  SAMPLES_POOL=1000000

# System prep step
RUN set -x \
  && useradd -c "app" -s /bin/bash -u 1000 -m app \
  && mkdir -p /home/app/.ssh \
  && ssh-keygen -t ed25519 -N "" -C openmpi -f /home/app/.ssh/id_ed25519 \
  && mv /home/app/.ssh/id_ed25519.pub /home/app/.ssh/authorized_keys \
  && chown -R app:app /app \
  && chown -R app:app /home/app \
  && chown -R app:app /app \
  && chown -R app:app /home/app \
  && chown -R app:app /etc/dropbear \
  && ln -sf /usr/bin/doas /usr/bin/sudo \
  && echo "permit nopass app" > /etc/doas.conf

# Additional copying of files to image
# and giving them permissions
COPY --chown=app:app ["hostfile.txt", "start.sh", "/app/"]
RUN set -x \
  && chmod +x /app/*.sh

USER app
CMD [ "dropbear", "-F", "-E", "-R" ]
