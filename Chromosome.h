#ifndef __CHROMOSOME_H__
#define __CHROMOSOME_H__

#include <vector>

class Chromosome {
public:
  int num_turbines;
  double fitness;
  std::vector<int> turbines;

  Chromosome(int num_turbines);
  ~Chromosome();

  bool operator<(const Chromosome &b) const;
};
#endif
