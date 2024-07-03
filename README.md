# connect-udp-QUIC

## Install dependencies
  - `sudo apt-get install gcc g++ zlib1g zlib1g-dev libunwind-dev libevent-dev`

## Install and add Golang (version>=1.19) to path
  - `sudo apt-get install "golang-1.20*"`
  - `sudo cp /usr/lib/go-1.20/bin/go /usr/bin/go`
  - `go version`

## Setup submodules
  - `git submodule update --init --recursive`

## Build the project
  - `mkdir build`
  - `cd build`
  - `cmake .. && make`

## Run example
  - `./http_proxy_server -c www.example.com,fullchain.pem,privkey.pem -s 0.0.0.0:4433`
  - `./http_proxy_client -H www.example.com -s 127.0.0.1:4433 -p /`
