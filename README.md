# Process Scheduling Simulator in C

This repository contains a simulation of process scheduling algorithms, including Round-Robin and Shortest Job First (SJF), implemented in C. This project was developed as a side project, inspired by the concepts I learned during my Computer Systems course while pursuing my Master's in Software Engineering.

## Overview

Process scheduling is a fundamental concept in operating systems, used to allocate CPU resources efficiently among multiple processes. This project simulates the process queue and implements the Round-Robin and SJF scheduling algorithms to demonstrate how processes are managed in an operating system.

## Features

- **Round-Robin Scheduling:** Implements a fixed time slice to simulate the Round-Robin scheduling of processes.
- **Shortest Job First (SJF) Scheduling:** Simulates non-preemptive SJF scheduling to prioritize shorter processes.
- **Process Queue Management:** Dynamically manages the process queue using basic data structures in C.
- **Console Output:** Provides detailed output of process states, time slices, and CPU utilization.

## Why Process Scheduling?

Understanding process scheduling is crucial for comprehending how operating systems manage resources and ensure fair process execution. This simulation provides a hands-on approach to exploring scheduling algorithms and their impact on process management.

## Getting Started

### Prerequisites

- GCC or any standard C compiler.
- Basic knowledge of C programming and process scheduling concepts.

### Compilation

To compile the project, use the following command:

```bash
make
