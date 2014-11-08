/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/statsd.h>
#include <fnordmetric/util/unittest.h>

using namespace fnordmetric::metricdb;

UNIT_TEST(StatsdTest);

using LabelList = std::vector<std::pair<std::string, std::string>>;

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormat, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric";
  auto ret = StatsdServer::parseStatsdSample(
      test_smpl.c_str(),
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT_EQ(ret, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "");
});

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormatWithValue, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric:34.23";
  auto ret = StatsdServer::parseStatsdSample(
      test_smpl.c_str(),
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT_EQ(ret, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "34.23");
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithLabels, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric[label1=435][l2=str]:34.23";
  auto ret = StatsdServer::parseStatsdSample(
      test_smpl.c_str(),
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT_EQ(ret, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 2);
  EXPECT_EQ(labels[0].first, "label1");
  EXPECT_EQ(labels[0].second, "435");
  EXPECT_EQ(labels[1].first, "l2");
  EXPECT_EQ(labels[1].second, "str");
  EXPECT_EQ(value, "34.23");
});