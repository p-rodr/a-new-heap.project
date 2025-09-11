# a-new-heap.project
Galactic Warfare Simulation (C++) – Designed a simulation engine with custom heaps and priority queues to resolve large-scale battles.

Here’s a polished **README.md** for your **A New Heap – Galactic Warfare Simulation** project. You can copy and paste this directly into your repository:

---

# A New Heap – Galactic Warfare Simulation

A **priority queue–based battle simulator** written in C++ as part of EECS 281 (Data Structures & Algorithms). This project, nicknamed **“A New Heap,”** models intergalactic conflicts between Jedi and Sith forces using **pairing heaps** and other priority queue implementations to efficiently manage combat events.

## ✨ Features

* Simulates large-scale battles between opposing forces with thousands of events.
* Implements **pairing heap** and compares performance with binary heap and STL priority queue.
* Supports multiple output modes (verbose, watcher, median, general evaluation) to test different scenarios.
* Handles ties, force alignment, and time-based event resolution according to specification rules.

## 🛠 Tech Stack

* **Language:** C++17
* **Data Structures:** Pairing Heap, Binary Heap, STL Priority Queue
* **Tools:** G++, Make (optional), Valgrind for memory checking.

### Build and Run

```bash
# Clone the repo
git clone https://github.com/p-rodr/a-new-heap.git
cd a-new-heap

# Compile
g++ -std=c++17 -O2 -o galactic main.cpp pairing_heap.cpp

# Run with sample input
./galactic < sample_input.txt > output.txt
```


## 🧠 What I Learned

* Implementing and analyzing the **pairing heap** priority queue.
* Managing complex event scheduling for simulations.
* Debugging and performance profiling with large test cases.
* Applying **Big-O analysis** to real-world scenarios.
