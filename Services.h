#ifndef Services_h
#define Services_h

int convert_to_minutes(const int& time_min)
{
  return (time_min) % 60;
};

static int convert_to_hour(const int& time_min)
{
  return (time_min - convert_to_minutes(time_min)) / 60;
};

#endif