/**
 * @author Masaki Waga
 * @date 2022/03/08.
 */

#include <iostream>
#include <boost/test/unit_test.hpp>

#define protected public

#include "../include/timed_automaton_runner.hh"
#include "../include/symbolic_membership_oracle.hh"

#include "simple_automaton_fixture.hh"
#include "simple_observation_table_keys_fixture.hh"

using namespace learnta;

struct SimpleAutomatonOracleFixture : public SimpleAutomatonFixture, public SimpleObservationTableKeysFixture {
  std::unique_ptr<learnta::SymbolicMembershipOracle> oracle;

  SimpleAutomatonOracleFixture() : SimpleAutomatonFixture(), SimpleObservationTableKeysFixture() {
    auto runner = std::unique_ptr<learnta::SUL>(new learnta::TimedAutomatonRunner{automaton});

    this->oracle = std::make_unique<learnta::SymbolicMembershipOracle>(std::move(runner));
  }
};

BOOST_AUTO_TEST_SUITE(SymbolicMembershipOracleTest)

  BOOST_FIXTURE_TEST_CASE(p4s1, SimpleAutomatonOracleFixture) {
    // Bottom
    BOOST_CHECK(this->oracle->query(p4 + s1).empty());
  }

  BOOST_FIXTURE_TEST_CASE(p5s1, SimpleAutomatonOracleFixture) {
    // Bottom
    BOOST_CHECK(this->oracle->query(p5 + s1).empty());
  }

  BOOST_FIXTURE_TEST_CASE(p4s3, SimpleAutomatonOracleFixture) {
    // Top
    auto p4s3 = p4 + s3;
    auto resultP4S3 = this->oracle->query(p4s3);
    BOOST_CHECK_EQUAL(1, resultP4S3.size());
    BOOST_CHECK_EQUAL(p4s3.timedCondition, resultP4S3.front());
  }

  BOOST_FIXTURE_TEST_CASE(p5s3, SimpleAutomatonOracleFixture) {
    // Top && 0 < tau1 <= 1
    auto p5s3 = p5 + s3;
    auto resultP5S3 = this->oracle->query(p5s3);
    auto expected = p5s3.timedCondition;
    expected.restrictUpperBound(1, 1, {1, true});
    expected.restrictLowerBound(1, 1, {0, false});
    BOOST_CHECK_EQUAL(1, resultP5S3.size());
    BOOST_CHECK_EQUAL(expected, resultP5S3.front());
  }

BOOST_AUTO_TEST_SUITE_END()
