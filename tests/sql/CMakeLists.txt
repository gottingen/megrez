#
# Copyright 2023 The titan-search Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


carbin_cc_test(
        NAME parser_for_dml_test
        MODULE sql
        SOURCES parser_for_dml_test.cc
        LINKS
        megrez::megrez
        GTest::gtest
        GTest::gtest_main
        ${CARBIN_DEPS_LINK}
        CXXOPTS ${CARBIN_CXX_OPTIONS}
)

carbin_cc_test(
        NAME parser_for_load_data_test
        MODULE sql
        SOURCES parser_for_load_data_test.cc
        LINKS
        megrez::megrez
        GTest::gtest
        GTest::gtest_main
        ${CARBIN_DEPS_LINK}
        CXXOPTS ${CARBIN_CXX_OPTIONS}
)

carbin_cc_test(
        NAME parser_for_select
        MODULE sql
        SOURCES parser_for_select.cc
        LINKS
        megrez::megrez
        GTest::gtest
        GTest::gtest_main
        ${CARBIN_DEPS_LINK}
        CXXOPTS ${CARBIN_CXX_OPTIONS}
)
