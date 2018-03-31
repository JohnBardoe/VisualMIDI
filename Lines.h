#pragma once

#ifndef LINEDEF

#define LINEDEF

class curve {
public:
	curve();
	vector<Point> points;
	int len;
	Scalar color;
};

class instrument {
public:
	instrument();
	instrument(int num);
	~instrument();
	int color;
	int eps;
	HSTREAM smp;
	string name;
};

#endif 

