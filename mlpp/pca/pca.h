
#ifndef MLPP_PCA_H
#define MLPP_PCA_H

//
//  PCA.hpp
//
//  Created by Marc Melikyan on 10/2/20.
//

#include "core/math/math_defs.h"

#include <vector>

class MLPPPCA {
public:
	std::vector<std::vector<real_t>> principalComponents();
	real_t score();

	MLPPPCA(std::vector<std::vector<real_t>> inputSet, int k);

private:
	std::vector<std::vector<real_t>> inputSet;
	std::vector<std::vector<real_t>> X_normalized;
	std::vector<std::vector<real_t>> U_reduce;
	std::vector<std::vector<real_t>> Z;
	int k;
};

#endif /* PCA_hpp */
