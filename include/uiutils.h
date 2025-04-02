#ifndef UIUTILS_H
#define UIUTILS_H

#include <raylib.h>
#include <string>
#include "Scheduler.h"

#define BGCOLOR (Color){23, 23, 23, 255}
#define BGCOLOR_ACTIVE (Color){40, 44, 52, 255}
#define FGCOLOR (Color){244, 221, 222, 255}
#define FGCOLOR_TAB (Color){184, 187, 241, 255}
#define CPUGREEN (Color){129, 178, 154, 255}
#define IORED (Color){224, 122, 95, 255}

#define HandleKeyPress(key, isPressed, action) \
  if (IsKeyDown(key) && !isPressed) {          \
    action;                                    \
    isPressed = true;                          \
  }                                            \
  if (IsKeyUp(key) && isPressed) {             \
    isPressed = false;                         \
  }

void DrawTableBoxes(int x,
                    int y,
                    int rows,
                    int cols,
                    int cellWidth,
                    int cellHeight);
std::string formatTo2(double d);
void ProcsTab(Processes procs);
void AlgoTabs(int selection);

class GanttChart {
 public:
  GanttChart() {}
  GanttChart(Processes&& procs, size_t ticks);
  void setProcs(Processes& procs, size_t ticks);
  void draw();
  void DrawArrival(size_t chartY, size_t aT);
  size_t draw_performace();

 private:
  Processes procs = {};
  size_t maxTicks = 1;

  int w = 1100, h = 600;
  int chartX = 100;
  int chartY = 150;
  int nameStart = 20;
  size_t units = 1100;

  void DrawChart(std::vector<std::pair<size_t, size_t>> bursts,
                 size_t chartY,
                 size_t y,
                 Color clr);
};
#endif  // UIUTILS_H
