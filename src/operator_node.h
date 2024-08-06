#pragma once
#include<iostream>
#include <vector>
#include <optional>

#include "condition.h"
using namespace std;

class OperatorNode : public Condition {
public:
	vector<Condition*> conditions;
	int countTrueConditions;

	OperatorNode() : countTrueConditions(0) {};
	optional<bool> updateTree();
	virtual void updateStatus() = 0;

};