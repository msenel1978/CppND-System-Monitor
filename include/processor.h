#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // DONE: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
  long user_time{0};
  long nice_time{0};
  long system_time{0};
  long idle_time{0};
  long iowait_time{0};
  long irq_time{0};
  long softirq_time{0};
  long steal_time{0};
  long guest_time{0};
  long guest_nice_time{0};
  long prev_busy{0};
  long prev_total{0};
};

#endif
