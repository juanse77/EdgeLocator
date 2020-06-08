#include "GaussianFilter.h"

namespace EdgeLocator {
	
	float GaussianFilter::A11 = 1.f / 20.f;
	float GaussianFilter::A01 = 3.f / 40.f;
	float GaussianFilter::A00 = 1.f / 2.f;

}