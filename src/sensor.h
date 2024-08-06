#pragma once
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <optional>
#include "basic_condition.h"
using namespace std;


class Sensor
{
public:
	int id;
	// code 700 [ ->, ->]
	map< string, pair<string, vector<BasicCondition*>> > fields;
	Sensor(int id) : id(id) { };
    //
	set<int> changeValueOfField(string field, string value);
   //
	virtual void doAction(string action) {};

};

