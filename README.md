# Network Protocols Simulation (C++)

A comprehensive simulation of Reliable Data Transfer (RDT) and TCP Congestion Control protocols, focusing on packet reliability, corruption handling, and network flow control.

## 🚀 Protocols Implemented

### 1. Reliable Data Transfer (RDT)
* **RDT 2.2**: Implements a stop-and-wait protocol that handles corrupted packets using checksums and ACKs with alternating sequence numbers (0 or 1).
* **RDT 3.0**: Simulates a channel with both packet loss and corruption. It features a **multi-threaded timer** implementation using `std::mutex` and `std::condition_variable` to handle retransmissions upon timeout.

### 2. TCP Congestion Control
* **TCP Tahoe**: Simulates the Slow Start and Congestion Avoidance phases. Upon detecting packet loss, the congestion window (`cwnd`) is reset to 1, and `ssthresh` is updated to half of the current `cwnd`.
* **TCP Reno**: Enhances the simulation by adding logic for **Triple Duplicate ACKs**. It distinguishes between timeouts (resetting `cwnd` to 1) and duplicate ACKs (performing fast recovery by setting `cwnd` to `cwnd/2`).

## 🛠️ Technical Implementation
* **Checksum Logic**: Validates data integrity by summing sequence numbers and ASCII values of the payload.
* **Unreliable Channel**: Simulates real-world network conditions using a `transmit` function with configurable loss and corruption probabilities (default set to 0.3).
* **Concurrency**: Utilized C++ threads in RDT 3.0 to manage non-blocking protocol timers.

## 📂 Repository Structure
* `/Transport-Layer`: C++ source code for RDT 2.2 and RDT 3.0.
* `/Congestion-Control`: C++ source code for TCP Tahoe and TCP Reno.
* `/docs`: Detailed assignment report containing logic analysis and output screenshots.

## ⚙️ How to Run
Compile the files using any C++11 compliant compiler. For RDT 3.0, ensure you link the pthread library:
```bash
g++ RDT_3.0.cpp -pthread -o rdt3
./rdt3
