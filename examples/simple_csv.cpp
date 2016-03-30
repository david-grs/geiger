#include "geiger/geiger.h"

#include <vector>
#include <cstdlib>

int main()
{
    geiger::init();
    geiger::suite<> s;

    s.add("rand", []()
          {
              std::rand();
          });

    s.add("vector push_back", []()
          {
              std::vector<int> v;
              v.push_back(1000);
          });

    // Redirection of each test result to out.csv
    s.set_printer(geiger::printer::csv("out.csv"));
    s.run();

    return 0;
}
