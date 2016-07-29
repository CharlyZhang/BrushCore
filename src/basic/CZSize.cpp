
#include "CZSize.h"

CZSize CZSize::operator*(float num) const 
{
	// Return the scaled vector
	return CZSize(width * num, height * num);
}