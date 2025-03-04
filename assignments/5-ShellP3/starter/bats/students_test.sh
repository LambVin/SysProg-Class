#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}
@test "Pipeline: echo splits words into separate lines" {
  run ./dsh <<EOF
echo "hello world from dsh" | tr ' ' '\n'
exit
EOF
  [[ "$output" == *"hello"* ]]
  [[ "$output" == *"world"* ]]
  [[ "$output" == *"from"* ]]
  [[ "$output" == *"dsh"* ]]
  [ "$status" -eq 249 ]
}

@test "Pipeline: sort numbers using three commands" {
  run ./dsh <<EOF
echo "3 1 2" | tr ' ' '\n' | sort -n
exit
EOF
  [[ "$output" == *"1"* ]]
  [[ "$output" == *"2"* ]]
  [[ "$output" == *"3"* ]]
  [ "$status" -eq 249 ]
}

@test "Pipeline: echo with extra spaces and quotes" {
  run ./dsh <<EOF
echo "   spaced   text   " | tr -s " "
exit
EOF
  [[ "$output" == *" spaced text"* ]]
  [ "$status" -eq 249 ]
}

@test "Pipeline: error on exceeding maximum pipe commands" {
  pipeline="echo test"
  for i in {1..9}; do
    pipeline="$pipeline | cat"
  done
  run ./dsh <<EOF
$pipeline
exit
EOF
  [[ "$output" == *"error: piping limited to 8 commands"* ]]
  [ "$status" -eq 249 ]
}

@test "Pipeline: non-existent command in pipeline returns error" {
  run ./dsh <<EOF
ls | nonexistentcommand
exit
EOF
  [[ "$output" == *"error: fail to execute external commands"* ]]
  [ "$status" -ne 0 ]
}

@test "Pipeline: handling empty command in pipeline" {
    run ./dsh <<EOF

EOF
    [[ "$output" == *"warning: no commands provided"* ]]
    [ "$status" -eq 0 ]
}
@test "Pipeline: multiple pipes with sorted output" {
  run ./dsh <<EOF
echo "c b a" | tr ' ' '\n' | sort
exit
EOF
  [[ "$output" == *"a"* ]]
  [[ "$output" == *"b"* ]]
  [[ "$output" == *"c"* ]]
  [ "$status" -eq 249 ]
}

@test "Pipeline: built-in cd used in pipeline should error" {
  run ./dsh <<EOF
echo dummy | cd
exit
EOF
  [[ "$output" == *"error: fail to execute external commands"* ]]
  [ "$status" -ne 0 ]
}

@test "Pipeline: echo with single quotes preserved then uppercase conversion" {
  run ./dsh <<EOF
echo 'single quoted test' | tr a-z A-Z
exit
EOF
  [[ "$output" == *"SINGLE QUOTED TEST"* ]]
  [ "$status" -eq 249 ]
}

@test "Pipeline: uname -a output filtered by grep" {
  run ./dsh <<EOF
uname -a | grep Linux
exit
EOF
  [[ "$output" == *"Linux"* ]]
  [ "$status" -eq 249 ]
}
