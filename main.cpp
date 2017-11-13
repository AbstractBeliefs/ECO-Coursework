//
//  main.cpp
//

#include <iostream>
#include <stdio.h>

#include "WindScenario.h"
#include "GA.h"
#include "KusiakLayoutEvaluator.h"

int main(int argc, const char * argv[]) {

  WindScenario wsc(argv[1]);  //  e.g.  "../Scenarios/practice_1.xml"
  KusiakLayoutEvaluator wfle;
  wfle.initialize(wsc);
  GA ga(wfle);
  ga.run();
}

