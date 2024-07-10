#!/bin/bash

cur_dir=$1

cd ${cur_dir} && flex sql_lex.l && bison sql_parse.y

dest_dir=$2
if [ ! -z "${dest_dir}" ]; then
  mv *.flex.* ${dest_dir}/ && mv *.yacc.* ${dest_dir}/
fi