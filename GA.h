#ifndef __GA_H__
#define __GA_H__

#include <vector>
#include "WindScenario.h"
#include "KusiakLayoutEvaluator.h"
#include "Matrix.hpp"
#include "Chromosome.h"

class GA {
  public:
    KusiakLayoutEvaluator wfle;
    vector<Chromosome> pops;
    int num_pop;
    double* fits;
    int nt;
    int tour_size;
    double mut_rate;
    double cross_rate;
    Matrix<double>* grid;

    GA(KusiakLayoutEvaluator evaluator);
    ~GA();
    void run();

  private:
    void evaluate();
    double evaluate_single(Chromosome soln);
    Chromosome population_pick_one();
    Chromosome tournament_selection(int tournament_size);
    Chromosome crossover(Chromosome a, Chromosome b);
    Chromosome mutate(Chromosome c);
};

#endif
