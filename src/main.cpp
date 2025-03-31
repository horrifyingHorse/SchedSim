#include <raylib.h>
#include <cstddef>
#include <string>
#include <vector>

#include "Scheduler.h"
#include "uiutils.h"

int main(int argc, char* argv[]) {
  int h = 800, w = 1300;
  InitWindow(w, h, "SchedSim");
  SetTargetFPS(60);

  Schedulers schedulers = {Scheduler::SJF, Scheduler::SRTF, Scheduler::RR,
                           Scheduler::VRR};
  std::vector<GanttChart> charts = {};
  std::vector<PerformanceReport> preports = {};
  Processes procs = GetProcs();
  Device d;
  for (const auto& scheduler : schedulers) {
    d.init(procs, {.sched = scheduler, .q = 5});
    d.start();
    charts.push_back(GanttChart(d.getCompletedProcs(), d.getTicks()));
    preports.push_back(d.getPerfReport());
  }

  int chartSelection = 0;
  bool isPressedL = false, isPressedH = false;
  bool isPressedJ = false, isPressedK = false;
  typedef struct {
    int x, y, length;
  } Tabs;
  Tabs activeTab[3] = {{.x = 5, .y = 5, .length = 90},
                       {.x = 105, .y = 5, .length = ((20 / 2) * 14) + 20},
                       {.x = 275, .y = 5, .length = ((20 / 2) * 14) + 25}};
  int activeTabNo = 0;
  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_Q)) {
      exit(0);
    }
    HandleKeyPress(
        KEY_J, isPressedJ, if (--activeTabNo < 0) { activeTabNo = 2; });
    HandleKeyPress(KEY_K, isPressedK, activeTabNo = (activeTabNo + 1) % 3);
    if (IsKeyDown(KEY_TWO)) {
      activeTabNo = 1;
    } else if (IsKeyDown(KEY_ONE)) {
      activeTabNo = 0;
    } else if (IsKeyDown(KEY_THREE)) {
      activeTabNo = 2;
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);
    DrawRectangle(0, 0, w, 25, BGCOLOR);
    DrawRectangle(activeTab[activeTabNo].x, activeTab[activeTabNo].y,
                  activeTab[activeTabNo].length, 25, BGCOLOR_ACTIVE);
    DrawText("1: Procs", 10, 10, 20, FGCOLOR_TAB);
    DrawText("2: Gantt Chart", 110, 10, 20, FGCOLOR_TAB);
    DrawText("3: Performance", 110 + (14 * 20 / 2) + 30, 10, 20, FGCOLOR_TAB);
    if (activeTabNo != 0) {
      AlgoTabs(chartSelection);
      HandleKeyPress(KEY_L, isPressedL,
                     chartSelection = (chartSelection + 1) % 4);
      HandleKeyPress(
          KEY_H, isPressedH, if (--chartSelection < 0) { chartSelection = 3; })
    }

    if (activeTabNo == 0) {
      ProcsTab(procs);
    } else if (activeTabNo == 1) {
      charts[chartSelection].draw();
    } else if (activeTabNo == 2) {
      size_t preportY = charts[chartSelection].draw_performace();
      PerformanceReport preport = preports[chartSelection];
      int i = 0;
      DrawText("Total Time Taken: ", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(
          std::string(std::to_string(preport.totalTicks) + " ticks").c_str(),
          400, preportY + 30 * i++, 20, FGCOLOR);
      DrawText("Avg Waiting Time: ", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(formatTo2(preport.avgWaitingTime).c_str(), 400,
               preportY + 30 * i++, 20, FGCOLOR);
      DrawText("Avg Turnaround Time:", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(formatTo2(preport.avgTurnAroundTime).c_str(), 400,
               preportY + 30 * i++, 20, FGCOLOR);
      DrawText("CPU Usage:", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(formatTo2(preport.cpuUsage).c_str(), 400, preportY + 30 * i++,
               20, FGCOLOR);
      DrawText("Throughput:", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(std::to_string(preport.throughput).c_str(), 400,
               preportY + 30 * i++, 20, FGCOLOR);
    }
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
