# `SchedSim`
Visualising and Simulating Processes under different Scheduling Algorithms in `Cpp`.<br>
for each `algorithm : algorithms`, we have:
- Gantt Chart
- Performance metrics like CPU usage, Throughput, Avg Waiting Time ...
- Waiting Time, Turnaround Time for each Process

### Algorithms Implemented:
1. Shortest Job First
2. Shortest Remaining Time First
3. Round Robin
4. Virtual Round Robin

## `./build`
> [!NOTE]
> _This project is built only for Linux System_

> Prerequisite to build this project: `cmake`

Simply execute the `build.sh` on your device, in your terminal paste:
```bash
chmod +x build.sh
./build.sh
```

## `procs.proc`
The processes are read from the `procs.proc` file, which **must** be present in the directory `./SchedSim` is being ran from.

Format of the `procs.proc` file:
```c
ProcName;ArrivalTime;CPUBurstTime;IOBurstTime;IOBurstRate
```

- `std::string` _ProcName_: Name of the Process
- `size_t` _ArrivalTime_: Arrival Time of the Process
- `size_t` _CPUBurstTime_: No. of CPU Bursts of the Process
- `size_t` _IOBurstTime_: No. of IO Bursts of the Process
- `size_t` _IOBurstRate_: After how many CPU Bursts, the Process goes for IO.
