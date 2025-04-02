#include "uiutils.h"
#include <raylib.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "Scheduler.h"

void DrawTableBoxes(int x,
                    int y,
                    int rows,
                    int cols,
                    int cellWidth,
                    int cellHeight) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      DrawRectangleLines(x + j * cellWidth, y + i * cellHeight, cellWidth,
                         cellHeight, BGCOLOR_ACTIVE);
    }
  }
}

std::string formatTo2(double d) {
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(2) << d;
  return ss.str();
}

void ProcsTab(Processes procs) {
  int balancey = 0;
  int x = 200;
  int y = 310;
  int i = 0;
  DrawText("SchedSim", x, 80, 60, FGCOLOR_TAB);
  DrawTableBoxes(x - 10, y - 10, procs.size() + 1, 5, 180, 40);

  DrawText("Process Name", (x + i++ * 180), y, 20, FGCOLOR);
  DrawText("Arrival Time", (x + i++ * 180), y, 20, FGCOLOR);
  DrawText("CPU Burst Time", (x + i++ * 180), y, 20, FGCOLOR);
  DrawText("IO Burst Time", (x + i++ * 180), y, 20, FGCOLOR);
  DrawText("IO Burst Rate", (x + i++ * 180), y, 20, FGCOLOR);
  for (const auto& proc : procs) {
    i = 0;
    balancey += 40;
    DrawText(proc.procName.c_str(), (x + i++ * 180), y + balancey, 20, FGCOLOR);
    DrawText(std::to_string(proc.arrivalTime).c_str(), (x + i++ * 180),
             y + balancey, 20, FGCOLOR);
    DrawText(std::to_string(proc.burstTimeCPU).c_str(), (x + i++ * 180),
             y + balancey, 20, FGCOLOR);
    DrawText(std::to_string(proc.burstTimeIO).c_str(), (x + i++ * 180),
             y + balancey, 20, FGCOLOR);
    DrawText(std::to_string(proc.burstTimeRate).c_str(), (x + i++ * 180),
             y + balancey, 20, FGCOLOR);
  }
}

void AlgoTabs(int selection) {
  DrawText("sjf", 10, 40, 20, (selection == 0) ? CPUGREEN : FGCOLOR_TAB);
  DrawText("srtf", 110, 40, 20, (selection == 1) ? CPUGREEN : FGCOLOR_TAB);
  DrawText("rr", 210, 40, 20, (selection == 2) ? CPUGREEN : FGCOLOR_TAB);
  DrawText("vrr", 310, 40, 20, (selection == 3) ? CPUGREEN : FGCOLOR_TAB);
}

GanttChart::GanttChart(Processes&& procs, size_t ticks) {
  setProcs(procs, ticks);
}

void GanttChart::setProcs(Processes& procs, size_t ticks) {
  this->procs = procs;
  maxTicks = ticks;
  units = w / maxTicks;
}

void GanttChart::draw() {
  size_t chartY = this->chartY;
  DrawLine(chartX, chartY, chartX + w, chartY, FGCOLOR);
  DrawLine(chartX, chartY, chartX, chartY + 40 + procs.size() * 60, FGCOLOR);
  for (size_t i = 0; i <= units; i++) {
    DrawText(std::to_string(i * maxTicks / units).c_str(),
             chartX + maxTicks * i, chartY - 25, 20, FGCOLOR);
  }
  DrawRectangle(w - chartX - 100, chartY / 3, 30, 20, CPUGREEN);
  DrawText("CPU Burst", w - chartX - 100 + 35, chartY / 3, 20, CPUGREEN);
  DrawRectangle(w - chartX + 100, chartY / 3, 30, 20, IORED);
  DrawText("IO Burst", w - chartX + 135, chartY / 3, 20, IORED);
  chartY += 40;
  for (auto& proc : procs) {
    double fsize = 20, fadjust = 1;
    while (proc.procName.size() * (double)((fsize / fadjust) / 2) + nameStart >=
           chartX) {
      fadjust *= 1.2;
      nameStart = std::max(nameStart - 3, 0);
    }
    DrawText(proc.procName.c_str(), nameStart, chartY - (fsize / fadjust) / 2,
             fsize / fadjust, FGCOLOR);
    DrawLine(chartX, chartY, chartX + w, chartY, FGCOLOR);
    DrawChart(proc.burstCPU, chartY, chartY - 20, CPUGREEN);
    DrawChart(proc.burstIO, chartY, chartY, IORED);
    DrawArrival(chartY, proc.arrivalTime);
    chartY += 60;
  }
}

size_t GanttChart::draw_performace() {
  DrawText("Performance Report:", 5, 80, 20, IORED);
  int adjustment = 0;
  DrawTableBoxes(90, chartY - 10, procs.size() + 1, 6, 180, 40);
  DrawText("Process Name", 100, chartY, 20, FGCOLOR);
  DrawText("Arrival Time", 280, chartY, 20, FGCOLOR);
  DrawText("Start Time", 460, chartY, 20, FGCOLOR);
  DrawText("Comp Time", 640, chartY, 20, FGCOLOR);
  DrawText("Turnaround Time", 820, chartY, 20, FGCOLOR);
  DrawText("Waiting Time", 1030, chartY, 20, FGCOLOR);
  for (auto& proc : procs) {
    DrawText(proc.procName.c_str(), 100, 190 + adjustment, 20, FGCOLOR);
    DrawText(std::to_string(proc.arrivalTime).c_str(), 280, 190 + adjustment,
             20, FGCOLOR);
    DrawText(std::to_string(proc.startTime).c_str(), 460, 190 + adjustment, 20,
             FGCOLOR);
    DrawText(std::to_string(proc.completionTime).c_str(), 640, 190 + adjustment,
             20, FGCOLOR);
    DrawText(std::to_string(proc.turnAroundTime()).c_str(), 820,
             190 + adjustment, 20, FGCOLOR);
    DrawText(std::to_string(proc.waitingTime()).c_str(), 1030, 190 + adjustment,
             20, FGCOLOR);
    adjustment += 40;
  }
  return 190 + adjustment + 20;
}

void GanttChart::DrawChart(std::vector<std::pair<size_t, size_t>> bursts,
                           size_t chartY,
                           size_t y,
                           Color clr) {
  Vector2 mousePos = GetMousePosition();
  for (auto& burst : bursts) {
    int x = chartX + burst.first * units;
    int len = (burst.second - burst.first) * units;
    int h = 20;
    Rectangle r = {(float)x, (float)y, (float)len, (float)h};
    Color color = clr;
    if (CheckCollisionPointRec(mousePos, r)) {
      DrawText(std::string(std::to_string(burst.first) + "-" +
                           std::to_string(burst.second))
                   .c_str(),
               x, y - 25, 20, FGCOLOR);
      color = SKYBLUE;
    }
    DrawRectangleRec(r, color);
  }
}

void GanttChart::DrawArrival(size_t chartY, size_t aT) {
  float x = chartX + aT * units;
  float y = chartY;
  Rectangle r = {x, y - 10, 5, 20};
  DrawRectangleRec(r, SKYBLUE);
  Vector2 mousePos = GetMousePosition();
  if (CheckCollisionPointRec(mousePos, r)) {
    DrawText(std::string(std::to_string(aT)).c_str(), x, y - 30, 20, FGCOLOR);
  }
}
