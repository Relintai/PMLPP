
#ifndef MLPP_OUTLIER_FINDER_H
#define MLPP_OUTLIER_FINDER_H

//
//  OutlierFinder.hpp
//
//  Created by Marc Melikyan on 11/13/20.
//

#include <vector>


class MLPPOutlierFinder {
public:
	// Cnstr
	MLPPOutlierFinder(int threshold);

	std::vector<std::vector<double>> modelSetTest(std::vector<std::vector<double>> inputSet);
	std::vector<double> modelTest(std::vector<double> inputSet);

	// Variables required
	int threshold;
};


#endif /* OutlierFinder_hpp */