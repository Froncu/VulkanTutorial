#include "HelperStructs.h"

bool vul::QueueFamilyIndices::isComplete() const
{
	return
		graphics.has_value() and
		present.has_value();
}