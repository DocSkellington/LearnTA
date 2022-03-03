/**
 * @author Masaki Waga
 * @date 2022/03/03.
 */
#define protected public

#include "../include/timed_condition.hh"
using namespace learnta;

#include <boost/test/unit_test.hpp>




BOOST_AUTO_TEST_SUITE(TimedConditionTest)

  using namespace learnta;

  BOOST_AUTO_TEST_CASE(empty) {
    TimedCondition empty = TimedCondition::empty();
    BOOST_TEST(empty.isSimple());
  }

  BOOST_AUTO_TEST_CASE(concatenate) {
    TimedCondition left = TimedCondition::empty();
    TimedCondition right = TimedCondition::empty();

    // left is \tau_0 \in (0,1) && \tau_0 + \tau_1 = 1 && \tau_1 \in (0,1)
    // Our encoding is x0 == 0, x1 == \tau_0 + \tau_1, and x2 == \tau_1.
    // Therefore, we have x1 - x2 < 1 && x2 - x1 < 0 && x1 - x0 <= 1 && x0 - x1 <= -1 && x2 - x0 < 1 && x0 - x2 < 0
    left.zone = Zone::top(3);
    left.zone.tighten(0, 1, {1, false}); // x1 - x2 < 1
    left.zone.tighten(1, 0, {0, false}); // x2 - x1 < 0
    left.zone.tighten(0, -1, {1, true}); // x1 - x0 <= 1
    left.zone.tighten(-1, 0, {-1, true}); // x0 - x1 <= -1
    left.zone.tighten(1, -1, {1, false}); // x2 - x0 < 1
    left.zone.tighten(-1, 1, {0, false}); // x0 - x2 < 0

    // right is \tau_0 \in (0,1)
    right.zone = Zone::top(2);
    right.zone.tighten(0, -1, {1, false}); // x1 - x0 < 1
    right.zone.tighten(-1, 0, {0, false}); // x0 - x1 < 0

    TimedCondition result;
    left.concatenate(right, result);
    // result should be \tau_0 \in (0,1) && \tau_0 + \tau_1 = (1,2) && \tau_1 \in (0,2)
    // Our encoding is x0 == 0, x1 == \tau_0 + \tau_1, and x2 == \tau_1.
    // Therefore, we have x1 - x2 < 1 && x2 - x1 < 0 && x1 - x0 < 2 && x0 - x1 < -1 && x2 - x0 < 2 && x0 - x2 < 0
    BOOST_CHECK_EQUAL(2, result.size());
    BOOST_CHECK_EQUAL((Bounds{1, false}), result.zone.value(1, 2));
    BOOST_CHECK_EQUAL((Bounds{0, false}), result.zone.value(2, 1));
    BOOST_CHECK_EQUAL((Bounds{2, false}), result.zone.value(1, 0));
    BOOST_CHECK_EQUAL((Bounds{-1, false}), result.zone.value(0, 1));
    BOOST_CHECK_EQUAL((Bounds{2, false}), result.zone.value(2, 0));
    BOOST_CHECK_EQUAL((Bounds{0, false}), result.zone.value(0, 2));
  }

BOOST_AUTO_TEST_SUITE_END()
