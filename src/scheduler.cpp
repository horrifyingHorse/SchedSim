#include "../include/Scheduler.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <fstream>

Process::Process(const std::string& name,
                 size_t at,
                 size_t btCPU,
                 size_t btIO,
                 size_t btr) {
  procName = std::move(name);
  arrivalTime = at;
  burstTimeCPU = btCPU;
  burstRemainCPU = btCPU;
  burstTimeIO = btIO;
  burstTimeRate = btr;
}
Process::State Process::exec() {
  state = State::RUNNING;
  if (--burstRemainCPU <= 0) {
    state = State::TERMINATED;
  } else if (++lastIOBurst >= burstTimeRate) {
    refreshIOBurst();
    state = State::BLOCKED;
  }
  return state;
}
void Process::refreshIOBurst() {
  lastIOBurst = 0;
}
size_t Process::turnAroundTime() {
  return completionTime - arrivalTime;
}
size_t Process::waitingTime() {
  return turnAroundTime() - burstTimeCPU;
}
size_t Process::responseTime() {
  return startTime - arrivalTime;
}
void Process::cpuBurstPush(size_t from, size_t to) {
  burstCPU.push_back({from, to});
}
void Process::ioBurstPush(size_t from, size_t to) {
  burstIO.push_back({from, to});
}
void Process::debug() {
  for (auto& burst : burstCPU) {
    std::cout << burst.first << ":" << burst.second << "\t";
  }
  std::cout << "\n";
  for (auto& burst : burstIO) {
    std::cout << burst.first << ":" << burst.second << "\t";
  }
  std::cout << "\n";
}

void RegularQ::push(const Process& p) {
  q.push(p);
}
void RegularQ::pop() {
  q.pop();
}
Process RegularQ::top() const {
  return q.front();
}
bool RegularQ::empty() const {
  return q.empty();
}
void RegularQ::clear() {
  q = {};
}

RemainPriorityQ::RemainPriorityQ() : q(cmpRemain) {}
void RemainPriorityQ::push(const Process& p) {
  q.push(p);
}
void RemainPriorityQ::pop() {
  q.pop();
}
Process RemainPriorityQ::top() const {
  return q.top();
}
bool RemainPriorityQ::empty() const {
  return q.empty();
}
void RemainPriorityQ::clear() {
  while (!q.empty())
    q.pop();
}

BurstPriorityQ::BurstPriorityQ() : q(cmp) {}
void BurstPriorityQ::push(const Process& p) {
  q.push(p);
}
void BurstPriorityQ::pop() {
  q.pop();
}
Process BurstPriorityQ::top() const {
  return q.top();
}
bool BurstPriorityQ::empty() const {
  return q.empty();
}
void BurstPriorityQ::clear() {
  while (!q.empty())
    q.pop();
}

Device::Device() {
  readyQ = &regularReadyQ;
}
void Device::init(Processes& procs) {
  this->procs = procs;
  totalProc = procs.size();
}
void Device::init(Processes& procs, SchedInfo sInfo) {
  this->procs = procs;
  this->totalProc = procs.size();
  this->scheduler = sInfo.sched;
  this->timeQuantum = sInfo.q;

  if (scheduler == Scheduler::SJF) {
    this->readyQ = &burstReadyQ;
  } else if (scheduler == Scheduler::SRTF) {
    this->readyQ = &remainReadyQ;
  } else {
    this->readyQ = &regularReadyQ;
  }
}

void Device::start() {
  CleanUp();
  Processor();
}

void Device::debug() {
  for (auto& proc : completedProcs) {
    LOG_DEBUG(proc.procName, "Arrival Time:\t", proc.arrivalTime)
    LOG_DEBUG("", "Start Time:\t", proc.startTime)
    LOG_DEBUG("", "Response Time:\t", proc.responseTime())
    LOG_DEBUG("", "Completion Time:", proc.completionTime)
    LOG_DEBUG("", "Turnaround Time:", proc.turnAroundTime())
    LOG_DEBUG("", "Waiting Time:\t", proc.waitingTime() << "\n")
    proc.debug();
  }
  LOG("Avg Waiting Time", avgWaitingTime(), "");
  LOG("Avg Turnaround Time", avgTurnAroundTime(), "");
  LOG("Avg Response Time", avgResponseTime(), "");
  LOG("Ticks CPU Idle\t", ticksCPUIdle, "");
  LOG("Total Ticks CPU\t", ticksCPU, "");
  LOG("Total CPU Usage\t", usageCPU() << " %", "");
  LOG("CPU Trhoughput\t", throughput(), "");
}

double Device::avgWaitingTime() {
  double sum = 0;
  for (auto& proc : completedProcs) {
    sum += proc.waitingTime();
  }
  return (double)(sum / completedProcs.size());
}

double Device::avgTurnAroundTime() {
  double sum = 0;
  for (auto& proc : completedProcs) {
    sum += proc.turnAroundTime();
  }
  return (double)(sum / completedProcs.size());
}

double Device::avgResponseTime() {
  double sum = 0;
  for (auto& proc : completedProcs) {
    sum += proc.responseTime();
  }
  return (double)(sum / completedProcs.size());
}

double Device::usageCPU() {
  return ((double)(ticksCPU - ticksCPUIdle) / ticksCPU) * 100;
}

double Device::throughput() {
  return (double)completedProcs.size() / ticksCPU;
}

size_t Device::getTicks() {
  return ticksCPU;
}

Processes Device::getCompletedProcs() {
  return completedProcs;
}

PerformanceReport Device::getPerfReport() {
  PerformanceReport pr;
  pr.cpuUsage = usageCPU();
  pr.throughput = throughput();
  pr.totalTicks = ticksCPU;
  pr.avgWaitingTime = avgWaitingTime();
  pr.avgTurnAroundTime = avgTurnAroundTime();
  return pr;
}

void Device::CleanUp() {
  execProc = {};
  completedProcs = {};
  q = 0;
  ticksCPU = 0;
  ticksCPUIdle = 0;
  isCPUIdle = true;

  countIOBurst = 0;
  isIOIdle = true;
  execProcIO = {};

  ioQ.clear();
  auxQ.clear();
  readyQ->clear();
}

void Device::Processor() {
  LOG("Time (tick)", "Device", "Process Served")
  bool executed;
  while (totalProc) {
    executed = false;
    LOG_TICK(ticksCPU)
    if (isCPUIdle) {
      LOG("\t", "CPU", "-");
    }

    FreshArrivals();
    if (!isCPUIdle) {
      executed = true;
      ExecCPU();
    }
    if (ToSchedule()) {
      ScheduleProc();
    }
    IODevice();

    if (!executed && isCPUIdle) {
      ticksCPUIdle++;
    }
    ticksCPU++;
    q++;
    LOG("", "", "")
  }
  ticksCPU--;
}

void Device::IODevice() {
  if (!isIOIdle) {
    ExecIO();
  }

  if (isIOIdle && !ioQ.empty()) {
    execProcIO = ioQ.top();
    ioQ.pop();
    countIOBurst = 0;
    fromIO = ticksCPU;
    isIOIdle = false;
    LOG("\t", "IO", execProcIO.procName << "[Sched]:" << countIOBurst)
  }
}

void Device::FreshArrivals() {
  int index = 0;
  for (auto& proc : procs) {
    if (proc.arrivalTime == ticksCPU) {
      LOG("\t", "CPU", proc.procName << "[Arrive]")
      proc.state = Process::State::READY;
      readyQ->push(proc);
      procs.erase(procs.begin() + index);
      continue;
    }
    index++;
  }
}

void Device::ExecCPU() {
  execProc.exec();
  if (execProc.state == Process::State::TERMINATED) {
    LOG("\t", "CPU", execProc.procName << "[Comp]");
    isCPUIdle = true;
    totalProc--;
    execProc.cpuBurstPush(fromCPU, ticksCPU);
    fromCPU = ticksCPU;
    execProc.completionTime = ticksCPU;
    completedProcs.push_back(execProc);
    execProc = {};
  } else if (execProc.state == Process::State::BLOCKED) {
    LOG("\t", "CPU", execProc.procName << "[Q IO]:" << execProc.burstRemainCPU);
    execProc.saveContextOfq = (timeQuantum != 0) ? (q + 1) % timeQuantum : 0;
    execProc.cpuBurstPush(fromCPU, ticksCPU);
    fromCPU = ticksCPU;
    ioQ.push(execProc);
    isCPUIdle = true;
    execProc = {};
  } else {
    LOG("\t", "CPU", execProc.procName << ":" << execProc.burstRemainCPU)
  }
}

void Device::ScheduleProc() {
  Process proc;
  if (!auxQ.empty()) {
    proc = auxQ.top();
    auxQ.pop();
    q = proc.saveContextOfq - 1;
  } else {
    proc = readyQ->top();
    readyQ->pop();
    q = -1;
  }
  LOG("\t", "CPU", proc.procName << "[Sched]")
  if (!isCPUIdle) {
    execProc.cpuBurstPush(fromCPU, ticksCPU);
    readyQ->push(execProc);
  }
  fromCPU = ticksCPU;
  execProc = proc;
  execProc.startTime = std::min(execProc.startTime, ticksCPU);
  isCPUIdle = false;
}

bool Device::ToSchedule() {
  switch (scheduler) {
    case Scheduler::SJF:
      return isCPUIdle && !readyQ->empty();
    case Scheduler::SRTF:
      return !readyQ->empty() && (isCPUIdle || readyQ->top().burstRemainCPU <
                                                   execProc.burstRemainCPU);
    case Scheduler::RR:
      return !readyQ->empty() && (isCPUIdle || q + 1 >= timeQuantum);
    case Scheduler::VRR:
      return (!readyQ->empty() || !auxQ.empty()) &&
             (isCPUIdle || q + 1 >= timeQuantum);
  }
  return false;
}

void Device::ExecIO() {
  if (++countIOBurst >= execProcIO.burstTimeIO) {
    LOG("\t", "IO", execProcIO.procName << "[Comp]:" << countIOBurst)
    execProcIO.ioBurstPush(fromIO, ticksCPU);
    fromIO = ticksCPU;
    if (scheduler == Scheduler::VRR) {
      auxQ.push(execProcIO);
    } else {
      readyQ->push(execProcIO);
    }
    execProcIO = {};
    isIOIdle = true;
  } else {
    LOG("\t", "IO", execProcIO.procName << ":" << countIOBurst)
  }
}

Processes GetProcs() {
#define GetIndex(exec)                           \
  if (indx != 0) {                               \
    sv.remove_prefix(indx + 1);                  \
  }                                              \
  indx = sv.find_first_of(";");                  \
  if (indx == std::string::npos) {               \
    std::cout << "Invalid Format in procs.proc"; \
    exit(1);                                     \
  }                                              \
  exec

  std::fstream procFile("procs.proc");
  if (!procFile) {
    std::cout << "Unable to open procs.proc";
    exit(1);
  }
  int indx = 0;
  Processes procs;
  std::string line;
  std::string_view sv;
  std::string procName;
  size_t arrivalTime = SIZE_MAX;
  size_t burstTimeCPU = SIZE_MAX;
  size_t burstTimeIO = SIZE_MAX;
  size_t burstTimeRate = SIZE_MAX;
  size_t burstRemainCPU = SIZE_MAX;

  while (std::getline(procFile, line)) {
    indx = 0;
    sv = line;
    GetIndex(procName = sv.substr(0, indx));
    GetIndex(arrivalTime = std::stoull((std::string)sv.substr(0, indx)));
    GetIndex(burstTimeCPU = std::stoull((std::string)sv.substr(0, indx)));
    GetIndex(burstTimeIO = std::stoull((std::string)sv.substr(0, indx)));
    sv.remove_prefix(indx + 1);
    burstTimeRate = std::stoull((std::string)sv);
    procs.push_back(Process(procName, arrivalTime, burstTimeCPU, burstTimeIO,
                            burstTimeRate));
  }
  return std::move(procs);
}
