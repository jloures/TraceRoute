# Traceroute

A simple traceroute implementation in C++ that traces the path packets take to a network host.

---

## Features

- Sends ICMP echo requests with increasing TTL to discover route hops.
- Displays the IP addresses and round-trip times for each hop.
- Basic error handling and timeout support.
- Compatible with Unix-like systems (Linux, macOS).

---

## Requirements

- C++17 compatible compiler (e.g., g++)
- Root or administrator privileges to send raw ICMP packets
- Networking headers and libraries installed

---

## Building

```bash
make
```

This will compile the source files and create the `traceroute` executable.

## Usage

```bash
sudo ./traceroute <hostname-or-ip>
```

### Example

```bash
sudo ./traceroute google.com
``` 
Note: Root privileges are usually required to send raw ICMP packets.







