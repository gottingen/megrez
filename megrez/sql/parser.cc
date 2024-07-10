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

#include <megrez/sql/parser.h>
#include "sql_lex.flex.h"

extern int sql_parse(yyscan_t scanner, megrez::SqlParser* parser);
namespace megrez {
void SqlParser::change_5c_to_7f(std::string& sql) {
    size_t i = 0;
    while (i < sql.size()) {
        if ((sql[i] & 0x80) != 0) {
            if (++i >= sql.size()) {
                return;
            }
            if (sql[i] == 0x5C) {
                sql[i] = 0x7F; // gbk second byte can not be 0x7F
                has_5c = true;
            }
        }
        ++i;
    }
}
void SqlParser::parse(const std::string& sql_) {
    std::string sql = sql_;
    if (charset == "gbk") {
        is_gbk = true;
    } else {
        is_gbk = false;
    }
    if (is_gbk) {
        change_5c_to_7f(sql);
    }
    yyscan_t scanner;
    sql_lex_init(&scanner);
    YY_BUFFER_STATE bp;
    bp = sql__scan_bytes(sql.c_str(), sql.size(), scanner);
    bp->yy_bs_lineno = 1;
    bp->yy_bs_column = 0;
    sql__switch_to_buffer(bp, scanner);
    sql_parse(scanner, this);
    sql__delete_buffer(bp, scanner);
    sql_lex_destroy(scanner);
}
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
