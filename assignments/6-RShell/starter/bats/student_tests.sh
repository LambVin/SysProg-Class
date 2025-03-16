#!/usr/bin/env bats

# File: student_tests.sh

setup() {
    export PORT=$((12345 + RANDOM % 1000))
    ./dsh -s -p $PORT &
    export SERVER_PID=$!
    sleep 1 
}

teardown() {
    kill $SERVER_PID 2>/dev/null || true
    wait $SERVER_PID 2>/dev/null || true
}

@test "Server starts and binds to port" {
    run nc -z localhost $PORT
    [ "$status" -eq 0 ]
}

@test "Client connects to server" {
    run ./dsh -c -i 127.0.0.1 -p $PORT <<< "exit"
    [ "$status" -eq 0 ]
}

@test "Exit command closes client connection" {
    run ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    
    run nc -z localhost $PORT
    [ "$status" -eq 0 ]
}

@test "stop-server command stops server" {
    run ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
stop-server
EOF
    [ "$status" -eq 0 ]
    
    # Verify server stopped
    run nc -z localhost $PORT
    [ "$status" -ne 0 ]
}

@test "Execute simple command via client" {
    run ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
echo test
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "test" ]]
}

@test "Pipeline command execution" {
    run ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
echo hello | wc -c
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "6" ]]
}

@test "Invalid command returns error" {
    run ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
invalidcommand
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "error" ]]
}

@test "cd command changes directory" {
    tmpdir=$(mktemp -d)
    run ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
cd $tmpdir
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "$tmpdir" ]]
    rmdir $tmpdir
}

@test "Multiple sequential clients" {
    run ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
echo first
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "first" ]]

    run ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
echo second
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "second" ]]
}
