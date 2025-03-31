#ifndef INCLUDE_INCLUDE_SCHEDULER_H_
#define INCLUDE_INCLUDE_SCHEDULER_H_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <queue>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifndef LOG_LEVEL_SCHEDSIM
#define LOG_LEVEL_SCHEDSIM 1  // Default log level 1
#endif

#define LOG_TICK(ticks)        \
  if (LOG_LEVEL_SCHEDSIM == 1) \
    std::cout << ticks << LOG_LEVEL_SCHEDSIM;
#define LOG(tick, device, procData) \
  if (LOG_LEVEL_SCHEDSIM == 1)      \
    std::cout << tick << "\t" << device << "\t\t" << procData << "\n";
#define LOG_DEBUG(name, label, info) \
  if (LOG_LEVEL_SCHEDSIM == 1)       \
    std::cout << name << "\n\t\t" << label "\t" << info;

typedef struct Process {
  enum State { READY, RUNNING, BLOCKED, TERMINATED };
  std::string procName;
  size_t arrivalTime = SIZE_MAX;
  size_t burstTimeCPU = SIZE_MAX;
  size_t burstTimeIO = SIZE_MAX;
  size_t burstTimeRate = SIZE_MAX;  // IO burst after every n CPU bursts
  size_t startTime = SIZE_MAX;
  size_t completionTime = SIZE_MAX;
  size_t burstRemainCPU = SIZE_MAX;
  size_t lastIOBurst = 0;
  long int saveContextOfq = 0;
  std::vector<std::pair<size_t, size_t>> burstCPU = {}, burstIO = {};
  State state;

  Process() {}
  Process(const std::string& name,
          size_t at,
          size_t btCPU,
          size_t btIO,
          size_t btr);
  State exec();
  void refreshIOBurst();
  size_t turnAroundTime();
  size_t waitingTime();
  size_t responseTime();
  void cpuBurstPush(size_t from, size_t to);
  void ioBurstPush(size_t from, size_t to);
  void debug();
} Process;
typedef std::vector<Process> Processes;

enum Scheduler { SJF, SRTF, RR, VRR };
typedef std::vector<Scheduler> Schedulers;

class IQueue {
 public:
  virtual ~IQueue() = default;
  virtual void push(const Process& p) = 0;
  virtual void pop() = 0;
  virtual Process top() const = 0;
  virtual bool empty() const = 0;
  virtual void clear() = 0;
};
class RegularQ : public IQueue {
 public:
  void push(const Process& p) override;
  void pop() override;
  Process top() const override;
  bool empty() const override;
  void clear() override;

 private:
  std::queue<Process> q = {};
};
class RemainPriorityQ : public IQueue {
 public:
  RemainPriorityQ();
  void push(const Process& p) override;
  void pop() override;
  Process top() const override;
  bool empty() const override;
  void clear() override;

 private:
  inline static auto cmpRemain = [](const Process& left, const Process& right) {
    if (left.burstRemainCPU != right.burstRemainCPU)
      return left.burstRemainCPU > right.burstRemainCPU;
    return left.arrivalTime > right.arrivalTime;
  };
  std::priority_queue<Process, Processes, decltype(cmpRemain)> q;
};
class BurstPriorityQ : public IQueue {
 public:
  BurstPriorityQ();
  void push(const Process& p) override;
  void pop() override;
  Process top() const override;
  bool empty() const override;
  void clear() override;

 private:
  inline static auto cmp = [](const Process& left, const Process& right) {
    if (left.burstTimeCPU != right.burstTimeCPU)
      return left.burstTimeCPU > right.burstTimeCPU;
    return left.arrivalTime > right.arrivalTime;
  };
  std::priority_queue<Process, Processes, decltype(cmp)> q;
};

typedef struct Performance {
  double avgWaitingTime = 0;
  double avgTurnAroundTime = 0;
  double cpuUsage = 0;
  double throughput = 0;
  size_t totalTicks = 0;
} PerformanceReport;

class Device {
 public:
  typedef struct SchedInfo {
    Scheduler sched = Scheduler::SJF;
    long int q = 0;
  } SchedInfo;

  Device();
  void init(Processes& procs);
  void init(Processes& procs, SchedInfo sInfo);
  void start();
  void debug();
  double avgWaitingTime();
  double avgTurnAroundTime();
  double avgResponseTime();
  double usageCPU();
  double throughput();
  size_t getTicks();
  Processes getCompletedProcs();
  PerformanceReport getPerfReport();

 private:
  Process execProc = {};
  Processes procs = {};
  Processes completedProcs = {};
  long int timeQuantum = 0;
  long int q = 0;
  size_t totalProc = 0;
  size_t ticksCPU = 0;
  size_t ticksCPUIdle = 0;
  size_t fromCPU = 0, fromIO = 0;
  bool isCPUIdle = true;
  Scheduler scheduler = Scheduler::SJF;

  Process execProcIO;
  bool isIOIdle = true;
  size_t countIOBurst = 0;
  IQueue* readyQ;
  BurstPriorityQ burstReadyQ;
  RemainPriorityQ remainReadyQ;
  RegularQ regularReadyQ;
  RegularQ ioQ, auxQ = {};

  void CleanUp();
  void Processor();
  void ExecCPU();
  void IODevice();
  void ExecIO();
  void FreshArrivals();
  bool ToSchedule();
  void ScheduleProc();
};

Processes GetProcs();
#endif  // INCLUDE_INCLUDE_SCHEDULER_H_
