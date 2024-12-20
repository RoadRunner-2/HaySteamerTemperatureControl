#ifndef DiagramDataSet_h
#define DiagramDataSet_h

#include <TimeLib.h>

struct DiagramDataSet {
    time_t current_time = 0;
    int temperature1 = 0;
    int temperature2 = 0;
};

class DiagramArray
{
  public:
    DiagramArray(){};

    void reset();

    bool add_data_point(time_t current_time, int temperature1, int temperature2)
    {
      if (index == 179) {
        return false; 
      }
      diagram_data[index].current_time = current_time;
      diagram_data[index].temperature1 = temperature1;
      diagram_data[index].temperature2 = temperature2;
      index++;
      return true;
    }

    int index = 0;
    DiagramDataSet diagram_data[180];
};

#endif
