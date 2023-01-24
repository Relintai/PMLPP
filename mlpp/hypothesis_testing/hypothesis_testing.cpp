//
//  HypothesisTesting.cpp
//
//  Created by Marc Melikyan on 3/10/21.
//

#include "hypothesis_testing.h"



std::tuple<bool, double> MLPPHypothesisTesting::chiSquareTest(std::vector<double> observed, std::vector<double> expected) {
	double df = observed.size() - 1; // These are our degrees of freedom
	double sum = 0;
	for (int i = 0; i < observed.size(); i++) {
		sum += (observed[i] - expected[i]) * (observed[i] - expected[i]) / expected[i];
	}
}

