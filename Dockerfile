FROM python:3.14

RUN apt-get update && \
  apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    curl \
    git \
  && rm -rf /var/lib/apt/lists/*

ARG USERNAME=builder
ARG USER_UID=1000
ARG USER_GID=1000
RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

WORKDIR /app


COPY . .

RUN chown -R $USERNAME:$USERNAME /app

RUN pip install --no-cache-dir --upgrade pip

USER $USERNAME
RUN python package.py

CMD ["/bin/bash"]
