#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef GENE_H_
#define GENE_H_

class Gene {
 private:
    bool doesSet;
    Automaton automaton;
    PreciseNumber score;
    
    void cross(const Automaton& mother,const Automaton& father);
    void mutation(const int rateOfMutation);
    void setExactName();
    bool doesActionsMeaningful() const;
 public:
    Gene(const Automaton& m);
    Gene(const Gene& mother,const Gene& father, const int rateOfMutation);

    Automaton getAutomaton() const;
    bool doesSetScore() const;
    void setScore(const PreciseNumber &value);
    PreciseNumber getScore() const;

    string toString() const;
};

bool operator<(const Gene &left, const Gene &right);
bool operator==(const Gene& left, const Gene &right);
ostream& operator<<(ostream &os, const Gene &gene);

#endif /* GENE_H_ */
