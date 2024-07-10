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
###########################################################################
# below are examples, after initialization, modify it to your own.
###########################################################################
###carbin_example


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/sql_gen)
file(COPY ${CMAKE_SOURCE_DIR}/megrez/sql DESTINATION ${CMAKE_BINARY_DIR}/sql_gen)
execute_process(
        COMMAND bash ${CMAKE_SOURCE_DIR}/megrez/gen_source.sh ${CMAKE_SOURCE_DIR}/megrez/sql ${CMAKE_SOURCE_DIR}/megrez/sql
        RESULT_VARIABLE CMD_RET
)

file(GLOB_RECURSE SQL_FILES "sql/*.cc")

carbin_cc_library(
        NAMESPACE megrez
        NAME megrez
        SOURCES ${SQL_FILES}
        CXXOPTS ${CARBIN_CXX_OPTIONS}
        PLINKS
        ${CARBIN_DEPS_LINK}
        PUBLIC
)
###carbin_example