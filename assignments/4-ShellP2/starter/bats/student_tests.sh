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

@test "Exit command returns status 249" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 249 ]
}

@test "Dragon builtin prints extra credit message" {
    run ./dsh <<EOF
dragon
exit
EOF
    echo "$output" | grep "DRAGON: extra credit not implemented"
    [ "$status" -eq 249 ]
}

@test "Pwd returns current directory" {
    current="$(pwd)"
    run ./dsh <<EOF
pwd
exit
EOF
    echo "$output" | grep "$current"
    [ "$status" -eq 249 ]
}

@test "Uname -a prints system information" {
    run ./dsh <<EOF
uname -a
exit
EOF
    echo "$output" | grep "Linux"
    [ "$status" -eq 249 ]
}

@test "Echo command with extra spaces in quotes" {
    run ./dsh <<EOF
echo "test    message with   spaces"
exit
EOF
    echo "$output" | grep "test    message with   spaces"
    [ "$status" -eq 249 ]
}

@test "Non-existent command returns error" {
    run ./dsh <<EOF
nonexistentcommand
exit
EOF
    [ "$status" -ne 0 ]
}

@test "Empty input line prints no commands provided" {
    run ./dsh <<EOF

exit
EOF
    echo "$output" | grep "no commands provided"
    [ "$status" -eq 249 ]
}

@test "Trailing spaces in ls command are handled" {
    run ./dsh <<EOF
ls    
exit
EOF
    [ "$status" -eq 249 ]
}

@test "Echo command with single quotes preserves content" {
    run ./dsh <<EOF
echo 'single quoted test'
exit
EOF
    echo "$output" | grep "single quoted test"
    [ "$status" -eq 249 ]
}

@test "Echo command with multiple arguments and extra spaces" {
    run ./dsh <<EOF
echo arg1   arg2    arg3
exit
EOF
    echo "$output" | grep "arg1 arg2 arg3"
    [ "$status" -eq 249 ]
}