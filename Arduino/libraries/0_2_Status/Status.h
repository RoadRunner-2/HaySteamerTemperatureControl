/*
  Status.h - A simple Arduino library defining the Status enum.
  Created by Eva Wiedner, 2025.
  Released under the MIT License.
*/

#ifndef Status_h
#define Status_h

enum Status
{
  idle = 0,
  ready,
  heating,
  holding,
  done, 
  error
};

#endif