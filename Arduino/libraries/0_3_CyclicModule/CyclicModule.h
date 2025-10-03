/*
  CyclicModule.h - A simple Arduino library defining an interface for cyclically called modules.
  Created by Eva Wiedner, 2025.
  Released under the MIT License.
*/

#ifndef CYCLICMODULE_H
#define CYCLICMODULE_H

class CyclicModule {
public:
	~CyclicModule() {};
	virtual void update() = 0;
};

#endif