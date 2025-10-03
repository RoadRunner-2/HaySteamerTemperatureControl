#pragma once

enum class Status
{
	idle = 0,
	ready,
	heating,
	holding,
	done,
	error
};