//
// Copyright (C) 2024 EA group inc.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <megrez/sql/dml.h>
#include "ddl.h"
#include "misc.h"

struct yy_buffer_state;

struct YYLTYPE {
    const char* start;
    const char* end;
};
#define YYLTYPE_IS_DECLARED 1  // userdefined YYLTYPE

#define YYLLOC_DEFAULT(Current, Rhs, N)                                   \
  do {                                                                    \
    if (N) {                                                              \
      (Current).start = YYRHSLOC(Rhs, 1).start;                       \
      (Current).end = YYRHSLOC(Rhs, N).end;                           \
    } else {                                                              \
      (Current).start = (Current).end = YYRHSLOC(Rhs, 0).end;             \
    }                                                                     \
  } while (0)
namespace megrez {
enum ParseError {
    SUCC = 0,
    SYNTAX_ERROR = 1,
};

struct SqlParser {
    std::string charset;
    std::string collation;
    std::vector<StmtNode*> result;
    ParseError error = SUCC;
    std::string syntax_err_str;
    turbo::Arena arena;
    bool is_gbk = false;
    bool has_5c = false;
    int place_holder_id = 0;
    void parse(const std::string& sql);
    void change_5c_to_7f(std::string& sql);
};

inline void print_stmt(Node* node, int ii = 0) {
    std::cout << "i:" << ii << " type:" << node->node_type << " ";
    node->print();
    if (node->node_type == NT_EXPR) {
        std::cout << "to_string:" << node << std::endl;
    }
    for (int i = 0; i < node->children.size(); i++) {
        print_stmt(node->children[i], ii + 1);
    }
}
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
