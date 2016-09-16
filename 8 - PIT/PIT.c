#include <PIT.h>
#include <IRQ.h>
#include <mode7.h>

void initPITHandler() {
  irq_install_handler(0, PITHandler);
}

void PITHandler(struct isrregs *r) {
  static uint32_t totalTicks = 0;
  uint32_t time_hours, time_minutes, time_seconds;

  //increase ticks by one
  totalTicks++;

  //roughly convert to total seconds
  time_seconds = totalTicks / 18;

  //hours
  time_hours = time_seconds / 3600;
  time_seconds %= 3600;

  //minutes
  time_minutes = time_seconds / 60;
  time_seconds %= 60;

  //draw clock
  drawClock(time_hours, time_minutes, time_seconds);
  
  return;
}

void drawClock(int hours, int minutes, int seconds) {
  //get current cursor positon
  uint32_t prev_xPos = GetXPos();
  uint32_t prev_yPos = GetYPos();

  //move cursor to top left
  GotoXY(0,0);

  //show hours
  if (hours <= 9) {
    DisplayInteger(0);
  }
  DisplayInteger(hours);
  DisplayChar(':');

  //show minutes
  if (minutes <= 9) {
    DisplayInteger(0);
  }
  DisplayInteger(minutes);
  DisplayChar(':');

  //show seconds
  if (seconds <= 9) {
    DisplayInteger(0);
  }
  DisplayInteger(seconds);
  
  //reset cursor to previous position
  GotoXY(prev_xPos, prev_yPos);
}
