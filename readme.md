# `SchedSim`
Visualising and Simulating Processes under different Scheduling Algorithms in `Cpp`.<br>

![Presentation1 (1)](https://github.com/user-attachments/assets/709a4a19-9f4a-429f-a719-7a4729771663)

for each `algorithm : algorithms`, we `SchedSim` produces:
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

> **Prerequisite** to build this project: `cmake`

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

## Navigating SchedSim
SchedSim supports vim motions like navigation.
- `j key`: Move to previous Tab
- `k key`: Move to next Tab
- `h key`: Move to previous Scheduling Algorithm
- `l key`: Move to next Scheduling Algorithm
> [!NOTE]
> You can also access a specific Tab by pressing the Tab number associated with that tab.
