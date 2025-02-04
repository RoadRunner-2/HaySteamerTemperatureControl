#ifndef Services_h
#define Services_h

enum InputMode
{
  none = 0,
  start_hour,
  start_minute,
  min_temp,
  min_time
};

enum Status
{
  idle = 0,
  ready,
  heating,
  holding_temperature,
  done, 
  error
};

int convert_to_minutes(const int& time_min)
{
  return (time_min) % 60;
};

static int convert_to_hour(const int& time_min)
{
  return (time_min - convert_to_minutes(time_min)) / 60;
};

#endif