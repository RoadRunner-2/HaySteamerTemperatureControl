# HaySteamerTemperatureControl
control a steam generator depending on the temperature in the hay

parameters of the steaming process - user level
- start time
- min temp
- duration

parameters of the steaming process - admin level
- min current
- max temp drop
- max heating time

faults handled:
- not enough current is used by the heating unit. might be due to any fault in the heating unit or a 
  safety shutdown because the water wasn't filled up (and the heating unit detected overheating)
  parameter: min current
- heating up takes too long, might be due to any number of reasons
  parameter: max heating time
- after the min temp was reached, the temperature start to drop again.
  parameter: max temp drop


functions:
- start timer: press the start timer button to signal ready for the system to start heating when reaching the start time.
  heating is started when the start time is reached the next time (might be the next day)
- start immediately: start heating immediately, runs the "normal" program, just start it immediately ignoring the start time
- continuous on: "shorts" the program, the relay is always on. disables all security measures


