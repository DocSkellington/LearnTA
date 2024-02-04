/**
 * @author Gaetan Staquet (F.R.S.-FNRS, University of Mons, University of Antwerp)
 * @date 2024/02/03.
 * @brief Implements the FDDI benchmark with a single station, based on Uppaal model.
 */

#include "experiment_runner.hh"

struct FDDISingleStation {
  /*!
   * The mapping of the alphabet from the original benchmark is as follows
   *
   * - TT: a
   * - RT: b
   * - @tau: c
   */
  const std::vector<learnta::Alphabet> alphabet = {'a', 'b', 'c'};
  learnta::TimedAutomaton targetAutomaton, complementTargetAutomaton;

  explicit FDDISingleStation(int SA, int TRTT) {
    using namespace learnta;

    // Define the set space
    targetAutomaton.states.resize(6);
    for (int i = 0; i < 6; ++i) {
      targetAutomaton.states.at(i) = std::make_shared<learnta::TAState>(true);
    }
    const auto Idle_z = targetAutomaton.states.at(0);
    const auto ST_z = targetAutomaton.states.at(1);
    const auto AT_z = targetAutomaton.states.at(2);
    const auto Idle_y = targetAutomaton.states.at(3);
    const auto ST_y = targetAutomaton.states.at(4);
    const auto AT_y = targetAutomaton.states.at(5);

    // Define the alphabet
    const learnta::Alphabet TT = 'a';
    const learnta::Alphabet RT = 'b';
    const learnta::Alphabet tau = 'c';

    // Define the clock variables
    const auto x = learnta::ConstraintMaker(0);
    const auto y = learnta::ConstraintMaker(1);
    const auto z = learnta::ConstraintMaker(2);
    // const auto t = learnta::ConstraintMaker(3);

    // Transitions
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(y, 0.0);
      resets.emplace_back(x, 0.0);
      Idle_z->next[TT].emplace_back(ST_z.get(), resets, std::vector<Constraint>{}); //std::vector<Constraint>{t <= 0, t >= 0});
    }
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(z, 0.0);
      resets.emplace_back(x, 0.0);
      Idle_y->next[TT].emplace_back(ST_y.get(), resets, std::vector<Constraint>{}); //std::vector<Constraint>{t <= 0, t >= 0});
    }
    {
      learnta::TATransition::Resets resets;
      // resets.emplace_back(t, 0.0);
      ST_z->next[RT].emplace_back(Idle_y.get(), resets, std::vector<Constraint>{x >= SA, z >= TRTT});
      AT_z->next[RT].emplace_back(Idle_y.get(), resets, std::vector<Constraint>{});
      ST_y->next[RT].emplace_back(Idle_z.get(), resets, std::vector<Constraint>{x >= SA, y >= TRTT});
      AT_y->next[RT].emplace_back(Idle_z.get(), resets, std::vector<Constraint>{});
    }
    {
      learnta::TATransition::Resets resets;
      ST_z->next[tau].emplace_back(AT_z.get(), resets, std::vector<Constraint>{x >= SA, z < TRTT});
      ST_y->next[tau].emplace_back(AT_y.get(), resets, std::vector<Constraint>{x >= SA, y >= TRTT});
    }

    std::cout << targetAutomaton << "\n";

    targetAutomaton.initialStates.push_back(Idle_z);
    targetAutomaton.maxConstraints = TimedAutomaton::makeMaxConstants(targetAutomaton.states);

    // simplify the target DTA
    targetAutomaton.simplifyStrong();
    targetAutomaton.simplifyWithZones();

    // Construct the complement DTA
    complementTargetAutomaton = targetAutomaton.complement(this->alphabet);
    complementTargetAutomaton.simplifyStrong();
    complementTargetAutomaton.simplifyWithZones();
  }
};

int main(int argc, const char *argv[]) {
#ifdef NDEBUG
  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#endif

  BOOST_LOG_TRIVIAL(info) << "Usage: " << argv[0] << " [SA] [TRTT]";
  if (argc == 1) {
    BOOST_LOG_TRIVIAL(info) << "Use the default SA (20) and TRTT (100)";
    FDDISingleStation single{20, 100};
    learnta::ExperimentRunner runner{single.alphabet, single.targetAutomaton};
    runner.run();
  } else {
    for (int i = 1; i < argc; i += 2) {
      const auto SA = atoi(argv[i]);
      int TRTT;
      if (i < argc - 1) {
        TRTT = atoi(argv[i+1]);
      } else {
        TRTT = 100;
      }
      BOOST_LOG_TRIVIAL(info) << "Use SA = " << argv[i] << " and TRTT = " << argv[i];
      FDDISingleStation single{SA, TRTT};
      learnta::ExperimentRunner runner{single.alphabet, single.targetAutomaton};
      runner.run();
    }
  }

  return 0;
}
