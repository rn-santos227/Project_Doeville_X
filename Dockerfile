FROM python:3.14

WORKDIR /app

RUN apt-get update && \
  apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    curl \
    git \
  && rm -rf /var/lib/apt/lists/*

COPY . .

RUN pip install --no-cache-dir --upgrade pip

RUN python package.py

CMD ["/bin/bash"]
