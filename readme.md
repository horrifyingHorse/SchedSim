# `SchedSim`
Visualising and Simulating Processes under different Scheduling Algorithms in `Cpp`.<br>

![Presentation1 (1)](https://github.com/user-attachments/assets/709a4a19-9f4a-429f-a719-7a4729771663)

for each `algorithm : algorithms`, `SchedSim` calculates:
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
> This project is compatible with Windows and Mac, CMakeLists.txt is provided and you can build it.

> [!NOTE]
> _Instructions below are only for Linux System_

> **Prerequisite** to build this project: `cmake`

Simply execute the `build.sh` on your device, in your terminal paste:
```python
chmod +x build.sh
./build.sh
```
**Compile Time Variable**: LOG_LEVEL_SCHEDSIM (defaults to 0), setting this to 1 will LOG every scheduler decision to the std out.
```python
cmake -DLOG_LEVEL_SCHEDSIM=1 ..   # set the LOG_LEVEL_SCHEDSIM at compile time
```

## `procs.proc`
The processes are read from the `procs.proc` file, which **must** be present in the directory `./SchedSim` is being ran from. An example `procs.proc` is provided by default in this project directory.

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
SchedSim does not support mouse clicks(yet), but you can hover over Gantt Chart bursts to find its execution time.

SchedSim only supports vim motions like navigation.
- `j key`: Move to previous Tab
- `k key`: Move to next Tab
- `h key`: Move to previous Scheduling Algorithm
- `l key`: Move to next Scheduling Algorithm

> [!TIP]
> You can also access a specific Tab by pressing the Tab number associated with that tab.

## ./history
The Scheduler that implements all the algorithms was built here: [SchedSim.cpp](https://github.com/horrifyingHorse/Segmentation-Fault-Dump/blob/main/OS/SchedSim.cpp)


The GUI implementation for the same was developed from scratch here: [trials/SchedSim](https://github.com/horrifyingHorse/trials/tree/main/raylib/SchedSim)

## A `cmake` rant
> [!TIP]
> TIP, I am the one who wants a tip. Enlighten me with why `FetchContent_declare` takes years to perform `git clone` of `raysan5/raylib`. Even if the shallow clone is set to true. I couldn't understant anything that was argued about slow FetchContent on online forums. Although any clever workarounds for the same are aprreciated.
>
> Including the entire source code and the build files of an external dependency in my project's source was the last thing i wanted to do, but here i am 😕
