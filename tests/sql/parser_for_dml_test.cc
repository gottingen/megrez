// Copyright (c) 2018 Baidu, Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <climits>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <megrez/sql/parser.h>

namespace parser {

TEST(test_parser, case_insert) {
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_insert0 = "insert LOW_PRIORITY ignore into db.table_a value (1, 'test'),"
            " (2, 'test_2'), (3, 'test_3') on duplicate key update"
            " db.table_a.field_a = 1, table_b.field_b = 2 ";
        parser.parse(sql_insert0);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_TRUE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_LOW_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_EQ(std::string(insert_stmt->table_name->db.value), "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(0, insert_stmt->columns.size());
        ASSERT_EQ(3, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(2, row_expr0->children.size());
        megrez::RowExpr* row_expr1 = insert_stmt->lists[1];
        ASSERT_EQ(2, row_expr1->children.size());
        megrez::RowExpr* row_expr2 = insert_stmt->lists[2];
        ASSERT_EQ(2, row_expr2->children.size());
        ASSERT_EQ(2, insert_stmt->on_duplicate.size());
        megrez::Assignment* assign0 = insert_stmt->on_duplicate[0];
        ASSERT_TRUE(assign0->name != nullptr);
        ASSERT_TRUE(std::string(assign0->name->db.value) == "db");
        ASSERT_TRUE(std::string(assign0->name->table.value) == "table_a");
        ASSERT_TRUE(std::string(assign0->name->name.value) == "field_a");
        ASSERT_TRUE(assign0->expr != nullptr);
        megrez::Assignment* assign1 = insert_stmt->on_duplicate[1];
        ASSERT_TRUE(assign1->name->db.empty());
        ASSERT_TRUE(std::string(assign1->name->table.value) == "table_b");
        ASSERT_TRUE(std::string(assign1->name->name.value) == "field_b");
        insert_stmt->set_print_sample(true);
        std::cout << "sql2: ";
        std::cout << insert_stmt->to_string() << std::endl;
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert1 = "insert delayed  table_a(table_a.field_a, field_b) "
            "value (1, 'test'), (2, 'test_2'), (3, 'test_3')"
            " on duplicate key update db.table_a.field_a := 1, table_b.field_b := 2 ";
        parser.parse(sql_insert1);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_DELAYED_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(insert_stmt->table_name->db.empty());
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(2, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->columns[0]->db.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->table.value) == "table_a");
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->name.value) == "field_a");

        ASSERT_TRUE(insert_stmt->columns[1]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[1]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[1]->name.value) == "field_b");
        
        ASSERT_EQ(3, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(2, row_expr0->children.size());
        megrez::RowExpr* row_expr1 = insert_stmt->lists[1];
        ASSERT_EQ(2, row_expr1->children.size());
        megrez::RowExpr* row_expr2 = insert_stmt->lists[2];
        ASSERT_EQ(2, row_expr2->children.size());
        ASSERT_EQ(2, insert_stmt->on_duplicate.size());
        megrez::Assignment* assign0 = insert_stmt->on_duplicate[0];
        ASSERT_TRUE(assign0->name != nullptr);
        ASSERT_TRUE(std::string(assign0->name->db.value) == "db");
        ASSERT_TRUE(std::string(assign0->name->table.value) == "table_a");
        ASSERT_TRUE(std::string(assign0->name->name.value) == "field_a");
        ASSERT_TRUE(assign0->expr != nullptr);
        megrez::Assignment* assign1 = insert_stmt->on_duplicate[1];
        ASSERT_TRUE(assign1->name->db.empty());
        ASSERT_TRUE(std::string(assign1->name->table.value) == "table_b");
        ASSERT_TRUE(std::string(assign1->name->name.value) == "field_b");
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert2 = "insert delayed  table_a(db.table_a.field_a)"
            " values (1, 'test') on duplicate key update"
            " db.table_a.field_a := 1, table_b.field_b := 2 ";
        parser.parse(sql_insert2);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_DELAYED_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(insert_stmt->table_name->db.empty());
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(1, insert_stmt->columns.size());
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->db.value) == "db");
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->table.value) == "table_a");
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->name.value) == "field_a");

        ASSERT_EQ(1, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(2, row_expr0->children.size());
        
        ASSERT_EQ(2, insert_stmt->on_duplicate.size());
        megrez::Assignment* assign0 = insert_stmt->on_duplicate[0];
        ASSERT_TRUE(assign0->name != nullptr);
        ASSERT_TRUE(std::string(assign0->name->db.value) == "db");
        ASSERT_TRUE(std::string(assign0->name->table.value) == "table_a");
        ASSERT_TRUE(std::string(assign0->name->name.value) == "field_a");
        ASSERT_TRUE(assign0->expr != nullptr);
        megrez::Assignment* assign1 = insert_stmt->on_duplicate[1];
        ASSERT_TRUE(assign1->name->db.empty());
        ASSERT_TRUE(std::string(assign1->name->table.value) == "table_b");
        ASSERT_TRUE(std::string(assign1->name->name.value) == "field_b");
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert3 = "insert high_priority  table_a values (1, 'test'), (2, 'test_2')"
            " on duplicate key update"
            " db.table_a.field_a := 1, table_b.field_b := values(field_a) + 10 ";
        parser.parse(sql_insert3);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_HIGH_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(insert_stmt->table_name->db.empty());
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(0, insert_stmt->columns.size());

        ASSERT_EQ(2, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(2, row_expr0->children.size());
        megrez::RowExpr* row_expr1 = insert_stmt->lists[1];
        ASSERT_EQ(2, row_expr1->children.size());
        
        ASSERT_EQ(2, insert_stmt->on_duplicate.size());
        megrez::Assignment* assign0 = insert_stmt->on_duplicate[0];
        ASSERT_TRUE(assign0->name != nullptr);
        ASSERT_TRUE(std::string(assign0->name->db.value) == "db");
        ASSERT_TRUE(std::string(assign0->name->table.value) == "table_a");
        ASSERT_TRUE(std::string(assign0->name->name.value) == "field_a");
        ASSERT_TRUE(assign0->expr != nullptr);
        megrez::Assignment* assign1 = insert_stmt->on_duplicate[1];
        ASSERT_TRUE(assign1->name->db.empty());
        ASSERT_TRUE(std::string(assign1->name->table.value) == "table_b");
        ASSERT_TRUE(std::string(assign1->name->name.value) == "field_b");
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert4 = "insert high_priority  table_a values (1), (2)"
            " on duplicate key update db.table_a.field_a := 1,"
            " table_b.field_b := values(field_a) + 10 ";
        parser.parse(sql_insert4);
        std::cout << sql_insert4 << "\n" << parser.syntax_err_str << "\n";
        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_HIGH_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(insert_stmt->table_name->db.empty());
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(0, insert_stmt->columns.size());

        ASSERT_EQ(2, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(1, row_expr0->children.size());
        megrez::RowExpr* row_expr1 = insert_stmt->lists[1];
        ASSERT_EQ(1, row_expr1->children.size());
        
        ASSERT_EQ(2, insert_stmt->on_duplicate.size());
        megrez::Assignment* assign0 = insert_stmt->on_duplicate[0];
        ASSERT_TRUE(assign0->name != nullptr);
        ASSERT_TRUE(std::string(assign0->name->db.value) == "db");
        ASSERT_TRUE(std::string(assign0->name->table.value) == "table_a");
        ASSERT_TRUE(std::string(assign0->name->name.value) == "field_a");
        ASSERT_TRUE(assign0->expr != nullptr);
        megrez::Assignment* assign1 = insert_stmt->on_duplicate[1];
        ASSERT_TRUE(assign1->name->db.empty());
        ASSERT_TRUE(std::string(assign1->name->table.value) == "table_b");
        ASSERT_TRUE(std::string(assign1->name->name.value) == "field_b");
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert5 = "insert high_priority  table_a values (1), (2)";
        parser.parse(sql_insert5);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_HIGH_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(insert_stmt->table_name->db.empty());
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(0, insert_stmt->columns.size());

        ASSERT_EQ(2, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(1, row_expr0->children.size());
        megrez::RowExpr* row_expr1 = insert_stmt->lists[1];
        ASSERT_EQ(1, row_expr1->children.size());
        
        ASSERT_EQ(0, insert_stmt->on_duplicate.size());
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert6 = "insert high_priority  table_a values (1)";
        parser.parse(sql_insert6);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_HIGH_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(insert_stmt->table_name->db.empty());
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(0, insert_stmt->columns.size());

        ASSERT_EQ(1, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(1, row_expr0->children.size());
        
        ASSERT_EQ(0, insert_stmt->on_duplicate.size());
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert7 = "insert LOW_PRIORITY ignore into db.table_a set field_a = 10,"
            " field_b = 100 on duplicate key update db.table_a.field_a = 1,"
            " table_b.field_b = values(field_a) + values(field_b) ";
        parser.parse(sql_insert7);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_TRUE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_LOW_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(std::string(insert_stmt->table_name->db.value) == "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(2, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->columns[0]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[0]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->name.value) == "field_a");

        ASSERT_TRUE(insert_stmt->columns[1]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[1]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[1]->name.value) == "field_b");

        ASSERT_EQ(1, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(2, row_expr0->children.size());
        
        ASSERT_EQ(2, insert_stmt->on_duplicate.size());
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert8 = "insert LOW_PRIORITY ignore into db.table_a set field_a = 10 "
            "on duplicate key update db.table_a.field_a = 1";
        parser.parse(sql_insert8);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_TRUE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_LOW_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(std::string(insert_stmt->table_name->db.value) == "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(1, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->columns[0]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[0]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->name.value) == "field_a");

        ASSERT_EQ(1, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(1, row_expr0->children.size());
        
        ASSERT_EQ(1, insert_stmt->on_duplicate.size());
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert9 = "insert into db.t1 select f1,f2,f3 from db.t2;";
        parser.parse(sql_insert9);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(std::string(insert_stmt->table_name->db.value) == "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "t1");
        ASSERT_EQ(0, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->subquery_stmt != nullptr);
        ASSERT_TRUE(insert_stmt->subquery_stmt->node_type == megrez::NT_SELECT);
        megrez::SelectStmt* select_stmt = (megrez::SelectStmt*)insert_stmt->subquery_stmt;
        ASSERT_EQ(3, select_stmt->fields.size());
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert9 = "insert into db.t1 (select f1,f2,f3 from db.t2);";
        parser.parse(sql_insert9);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(std::string(insert_stmt->table_name->db.value) == "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "t1");
        ASSERT_EQ(0, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->subquery_stmt != nullptr);
        ASSERT_TRUE(insert_stmt->subquery_stmt->node_type == megrez::NT_SELECT);
        megrez::SelectStmt* select_stmt = (megrez::SelectStmt*)insert_stmt->subquery_stmt;
        ASSERT_EQ(3, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->is_in_braces == true);
    }
    {
        megrez::SqlParser parser;
        std::string sql_insert9 = "insert into db.student1 (id,name1,name2,age1,age2,class1,class2,address,height)"
            "(select id,name1,name2,age1,age2,class1,class2,address,2 from student);";
        parser.parse(sql_insert9);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(std::string(insert_stmt->table_name->db.value) == "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "student1");
        ASSERT_EQ(9, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->subquery_stmt != nullptr);
        ASSERT_TRUE(insert_stmt->subquery_stmt->node_type == megrez::NT_SELECT);
        megrez::SelectStmt* select_stmt = (megrez::SelectStmt*)insert_stmt->subquery_stmt;
        ASSERT_EQ(9, select_stmt->fields.size());
        ASSERT_TRUE(select_stmt->is_in_braces == true);
    }
}
TEST(test_parser, case_merge) {
    {
        megrez::SqlParser parser;
        std::string sql_insert8 = "insert LOW_PRIORITY merge into db.table_a set field_a = 10 "
            "on duplicate key update db.table_a.field_a = 1";
        parser.parse(sql_insert8);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_FALSE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_TRUE(insert_stmt->is_merge);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_LOW_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_TRUE(std::string(insert_stmt->table_name->db.value) == "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(1, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->columns[0]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[0]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->name.value) == "field_a");

        ASSERT_EQ(1, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(1, row_expr0->children.size());
        
        ASSERT_EQ(1, insert_stmt->on_duplicate.size());
    }
}
TEST(test_parser, case_replace) {
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_replace0 = "replace LOW_PRIORITY into db.table_a(field_a, field_b) values (1, 'test'),"
            " (2, 'test_2'), (3, 'test_3')";
        parser.parse(sql_replace0);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_TRUE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_LOW_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_EQ(std::string(insert_stmt->table_name->db.value), "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(2, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->columns[0]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[0]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->name.value) == "field_a");

        ASSERT_TRUE(insert_stmt->columns[1]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[1]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[1]->name.value) == "field_b");
        

        ASSERT_EQ(3, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(2, row_expr0->children.size());
        megrez::RowExpr* row_expr1 = insert_stmt->lists[1];
        ASSERT_EQ(2, row_expr1->children.size());
        megrez::RowExpr* row_expr2 = insert_stmt->lists[2];
        ASSERT_EQ(2, row_expr2->children.size());
        ASSERT_EQ(0, insert_stmt->on_duplicate.size());
        insert_stmt->set_print_sample(true);
        std::cout << "sql2: ";
        std::cout << insert_stmt->to_string() << std::endl;
    }
}

TEST(test_parser, case_update) {
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_replace0 = "replace LOW_PRIORITY into db.table_a(field_a, field_b) values (1, 'test'),"
            " (2, 'test_2'), (3, 'test_3')";
        parser.parse(sql_replace0);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::InsertStmt));
        megrez::InsertStmt* insert_stmt = (megrez::InsertStmt*)parser.result[0];
        std::cout << insert_stmt->to_string() << std::endl;
        ASSERT_TRUE(insert_stmt->is_replace);
        ASSERT_FALSE(insert_stmt->is_ignore);
        ASSERT_EQ(insert_stmt->priority, megrez::PE_LOW_PRIORITY);
        ASSERT_TRUE(insert_stmt->table_name != nullptr);
        ASSERT_EQ(std::string(insert_stmt->table_name->db.value), "db");
        ASSERT_EQ(std::string(insert_stmt->table_name->table.value), "table_a");
        ASSERT_EQ(2, insert_stmt->columns.size());
        ASSERT_TRUE(insert_stmt->columns[0]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[0]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[0]->name.value) == "field_a");

        ASSERT_TRUE(insert_stmt->columns[1]->db.empty());
        ASSERT_TRUE(insert_stmt->columns[1]->table.empty());
        ASSERT_TRUE(std::string(insert_stmt->columns[1]->name.value) == "field_b");
        

        ASSERT_EQ(3, insert_stmt->lists.size());
        megrez::RowExpr* row_expr0 = insert_stmt->lists[0];
        ASSERT_EQ(2, row_expr0->children.size());
        megrez::RowExpr* row_expr1 = insert_stmt->lists[1];
        ASSERT_EQ(2, row_expr1->children.size());
        megrez::RowExpr* row_expr2 = insert_stmt->lists[2];
        ASSERT_EQ(2, row_expr2->children.size());
        ASSERT_EQ(0, insert_stmt->on_duplicate.size());
    }
}

TEST(test_parser, case_delete) {
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_delete0 = "delete low_priority quick ignore from db.table_a"
            " where filed_a + 3 > 100 or filed_b + 1 < 10 and filed_c *3 > 45"
            " order by filed_a, filed_b limit 10, 100";
        parser.parse(sql_delete0);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::DeleteStmt));
        megrez::DeleteStmt* delete_stmt = (megrez::DeleteStmt*)parser.result[0];
        std::cout << delete_stmt->to_string() << std::endl;
        ASSERT_TRUE(delete_stmt->is_ignore);
        ASSERT_EQ(delete_stmt->priority, megrez::PE_LOW_PRIORITY);
        ASSERT_TRUE(delete_stmt->is_quick);
        
        ASSERT_TRUE(typeid(*(delete_stmt->from_table)) == typeid(megrez::TableName));
        megrez::TableName* from_table = (megrez::TableName*)delete_stmt->from_table;
        ASSERT_EQ(std::string(from_table->db.value), "db"); 
        ASSERT_EQ(std::string(from_table->table.value), "table_a"); 
        delete_stmt->set_print_sample(true);
        std::cout << "sql2: ";
        std::cout << delete_stmt->to_string() << std::endl;
    }
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_delete1 = "delete from db.table_a"
            " where filed_a + 3 > 100 or filed_b + 1 < 10 and filed_c *3 > 45";
        parser.parse(sql_delete1);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::DeleteStmt));
        megrez::DeleteStmt* delete_stmt = (megrez::DeleteStmt*)parser.result[0];
        std::cout << delete_stmt->to_string() << std::endl;
        ASSERT_FALSE(delete_stmt->is_ignore);
        ASSERT_EQ(delete_stmt->priority, megrez::PE_NO_PRIORITY);
        ASSERT_FALSE(delete_stmt->is_quick);
        
        ASSERT_TRUE(typeid(*(delete_stmt->from_table)) == typeid(megrez::TableName));
        megrez::TableName* from_table = (megrez::TableName*)delete_stmt->from_table;
        ASSERT_EQ(std::string(from_table->db.value), "db"); 
        ASSERT_EQ(std::string(from_table->table.value), "table_a"); 
    }
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_delete2 = "delete db.table_a, db.table_b from db.table_a inner join db.table_b"
            " where filed_a + 3 > 100 or filed_b + 1 < 10 and filed_c *3 > 45";
        parser.parse(sql_delete2);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::DeleteStmt));
        megrez::DeleteStmt* delete_stmt = (megrez::DeleteStmt*)parser.result[0];
        std::cout << delete_stmt->to_string() << std::endl;
        ASSERT_FALSE(delete_stmt->is_ignore);
        ASSERT_EQ(delete_stmt->priority, megrez::PE_NO_PRIORITY);
        ASSERT_FALSE(delete_stmt->is_quick);
        
        ASSERT_TRUE(typeid(*(delete_stmt->from_table)) == typeid(megrez::JoinNode));
        
        ASSERT_EQ(2, delete_stmt->delete_table_list.size());
        megrez::TableName* table_name0 = delete_stmt->delete_table_list[0];
        ASSERT_EQ(std::string(table_name0->db.value), "db"); 
        ASSERT_EQ(std::string(table_name0->table.value), "table_a"); 
        megrez::TableName* table_name1 = delete_stmt->delete_table_list[1];
        ASSERT_EQ(std::string(table_name1->db.value), "db"); 
        ASSERT_EQ(std::string(table_name1->table.value), "table_b"); 
    }
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_delete2 = "delete db.table_a, db.table_b from db.table_a"
            " where filed_a + 3 > 100 or filed_b + 1 < 10 and filed_c *3 > 45";
        parser.parse(sql_delete2);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::DeleteStmt));
        megrez::DeleteStmt* delete_stmt = (megrez::DeleteStmt*)parser.result[0];
        std::cout << delete_stmt->to_string() << std::endl;
        ASSERT_FALSE(delete_stmt->is_ignore);
        ASSERT_EQ(delete_stmt->priority, megrez::PE_NO_PRIORITY);
        ASSERT_FALSE(delete_stmt->is_quick);
        
        ASSERT_TRUE(typeid(*(delete_stmt->from_table)) == typeid(megrez::TableSource));
    }
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_update0 = "update low_priority ignore db.table_a set filed_a = 10"
            " where filed_a + 3 > 100 or filed_b + 1 < 10 and filed_c *3 > 45 order by field_a, field_b desc limit 10";
        parser.parse(sql_update0);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::UpdateStmt));
        megrez::UpdateStmt* update_stmt = (megrez::UpdateStmt*)parser.result[0];
        std::cout << update_stmt->to_string() << std::endl;
        ASSERT_TRUE(update_stmt->is_ignore);
        ASSERT_EQ(update_stmt->priority, megrez::PE_LOW_PRIORITY);
        
        ASSERT_TRUE(typeid(*(update_stmt->table_refs)) == typeid(megrez::TableSource));
        update_stmt->set_print_sample(true);
        std::cout << "sql2: ";
        std::cout << update_stmt->to_string() << std::endl;
    }
    {
        megrez::SqlParser parser;
        //select distict
        std::string sql_truncate = "truncate db_table.a";
        parser.parse(sql_truncate);

        ASSERT_EQ(0, parser.error);
        ASSERT_EQ(1, parser.result.size());
        ASSERT_TRUE(typeid(*(parser.result[0])) == typeid(megrez::TruncateStmt));
        megrez::TruncateStmt* truncate_stmt = (megrez::TruncateStmt*)parser.result[0];
        std::cout << truncate_stmt->to_string() << std::endl;
        
        ASSERT_TRUE(typeid(*(truncate_stmt->table_name)) == typeid(megrez::TableName));
    }
}
} //namespace
