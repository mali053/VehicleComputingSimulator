#include "root.h"

void Root::updateTree()
{
	GlobalProperties& instanceGP = GlobalProperties::getInstance();

	if (this->countTrueConditions){
		instanceGP.trueConditions.insert(this->conditionId);
		this->status = true;
	}
	else{
		instanceGP.trueConditions.erase(this->conditionId);
		this->status = true;
	}
}
