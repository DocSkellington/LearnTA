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

  explicit FDDISingleStation(int SA, int TTRT) {
    using namespace learnta;

    // Define the set space
    targetAutomaton.states.resize(9);
    for (int i = 0; i < 9; ++i) {
      targetAutomaton.states.at(i) = std::make_shared<learnta::TAState>(true);
    }
    const auto Idle = targetAutomaton.states.at(0);
    const auto ST_y = targetAutomaton.states.at(1);
    const auto Idle_x = targetAutomaton.states.at(2);
    const auto ST_x_y = targetAutomaton.states.at(3);
    const auto AT_x_y = targetAutomaton.states.at(4);
    const auto Idle_y = targetAutomaton.states.at(5);
    const auto ST_x = targetAutomaton.states.at(6);
    const auto ST_y_x = targetAutomaton.states.at(7);
    const auto AT_y_x = targetAutomaton.states.at(8);

    // Define the alphabet
    const learnta::Alphabet TT = 'a';
    const learnta::Alphabet RT = 'b';
    const learnta::Alphabet tau = 'c';

    // Define the clock variables
    const auto x = learnta::ConstraintMaker(0);
    const auto y = learnta::ConstraintMaker(1);
    // const auto t = learnta::ConstraintMaker(2); // To force immediate TT-transitions

    // Transitions
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(y, 0.0);
      Idle->next[TT].emplace_back(ST_y.get(), resets, std::vector<Constraint>{}); //t <= 0, t >= 0});
    }
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(x, 0.0);
      // resets.emplace_back(t, 0.0);
      ST_y->next[RT].emplace_back(Idle_x.get(), resets, std::vector<Constraint>{y <= SA, y >= SA});
    }
    {
      learnta::TATransition::Resets resets;
      // resets.emplace_back(t, 0.0);
      Idle_x->next[tau].emplace_back(Idle.get(), resets, std::vector<Constraint>{x <= TTRT, x >= TTRT}); //, t <= 0, t >= 0});
    }
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(y, 0.0);
      Idle_x->next[TT].emplace_back(ST_x_y.get(), resets, std::vector<Constraint>{x < TTRT}); //t <= 0, t >= 0});
    }
    {
      learnta::TATransition::Resets resets;
      ST_x_y->next[tau].emplace_back(ST_y.get(), resets, std::vector<Constraint>{x <= TTRT, x >= TTRT});
    }
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(y, 0.0);
      ST_x_y->next[tau].emplace_back(AT_x_y.get(), resets, std::vector<Constraint>{y <= SA, y >= SA});
    }
    {
      learnta::TATransition::Resets resets;
      //resets.emplace_back(t, 0.0);
      AT_x_y->next[RT].emplace_back(Idle_y.get(), resets, std::vector<Constraint>{x <= TTRT});
    }
    {
      learnta::TATransition::Resets resets;
      //resets.emplace_back(t, 0.0);
      Idle_y->next[tau].emplace_back(Idle.get(), resets, std::vector<Constraint>{y <= TTRT, y >= TTRT}); //, t <= 0, t >= 0});
    }
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(x, 0.0);
      Idle_y->next[TT].emplace_back(ST_y_x.get(), resets, std::vector<Constraint>{y < TTRT}); //t <= 0, t >= 0});
    }
    {
      learnta::TATransition::Resets resets;
      ST_y_x->next[tau].emplace_back(ST_x.get(), resets, std::vector<Constraint>{y <= TTRT, y >= TTRT});
    }
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(y, 0.0);
      // resets.emplace_back(t, 0.0);
      ST_x->next[RT].emplace_back(Idle_y.get(), resets, std::vector<Constraint>{x <= SA, x >= SA});
    }
    {
      learnta::TATransition::Resets resets;
      resets.emplace_back(x, 0.0);
      ST_y_x->next[tau].emplace_back(AT_y_x.get(), resets, std::vector<Constraint>{x <= SA, x >= SA});
    }
    {
      learnta::TATransition::Resets resets;
      // resets.emplace_back(t, 0.0);
      AT_y_x->next[tau].emplace_back(Idle_x.get(), resets, std::vector<Constraint>{y <= TTRT});
    }

    targetAutomaton.initialStates.push_back(Idle);
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

  BOOST_LOG_TRIVIAL(info) << "Usage: " << argv[0] << " [SA] [TTRT]";
  if (argc == 1) {
    BOOST_LOG_TRIVIAL(info) << "Use the default SA (20) and TTRT (100)";
    FDDISingleStation single{20, 100};
    learnta::ExperimentRunner runner{single.alphabet, single.targetAutomaton};
    runner.run();
  } else {
    for (int i = 1; i < argc; i += 2) {
      const auto SA = atoi(argv[i]);
      int TTRT;
      if (i < argc - 1) {
        TTRT = atoi(argv[i+1]);
      } else {
        TTRT = 100;
      }
      BOOST_LOG_TRIVIAL(info) << "Use SA = " << argv[i] << " and TTRT = " << argv[i];
      FDDISingleStation single{SA, TTRT};
      learnta::ExperimentRunner runner{single.alphabet, single.targetAutomaton};
      runner.run();
    }
  }

  return 0;
}
