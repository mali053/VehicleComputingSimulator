#include "root.h"

void Root::updateTree()
{
	GlobalProperties& instanceGP = GlobalProperties::getInstance();

	if (this->countTrueConditions)
		instanceGP.trueConditions.insert(this->conditionId);
	else
		instanceGP.trueConditions.erase(this->conditionId);
}
