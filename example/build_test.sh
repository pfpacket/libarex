#!/bin/sh
#
#       example/build_test.sh
#   A simple script for building examples and tests
#
AREX_ROOT=`pwd`
BOOST_ROOT=~/boost/
CXX=g++
CXX_FLAGS="-std=c++0x -Wall"
CXX_INCL="-I $BOOST_ROOT/include/ -I $AREX_ROOT/include"
CXX_LIBS="-L $BOOST_ROOT/lib/ -lpthread -lboost_system"
EXEC_FILE=example.out
SIGNAL_HANDLER='clean_up_obj ; echo ; echo [!] Keyboard interrupt occurred \(SIGINT\)'
EXAMPLE_DIR_LIST=$(find $AREX_ROOT/example/* -type d)

clean_up_obj()
{
    find $AREX_ROOT/example/ -name "*.o" -exec rm -f {} \;
    find $AREX_ROOT/example/ -name $EXEC_FILE -exec rm -f {} \;
}

is_leaf_dir()
{
    RET=1
    [ "`find "$1" -maxdepth 1 -type d | wc -l`" = "1" ] && RET=0
    return $RET
}

if [ $? -ne 0 ]; then
    echo
    echo [-] Failed to find the directory of example/
    echo [-] Test failed ...compiling aborted
    exit 1
fi

trap "$SIGNAL_HANDLER" INT
for EXAMPLE_DIR in $EXAMPLE_DIR_LIST
do
    is_leaf_dir $EXAMPLE_DIR || continue
    ONE_EXAMPLE=$(find $EXAMPLE_DIR -name "*.cpp")
    for SRC in $ONE_EXAMPLE
    do
        echo [*] Compiling $SRC ...
        $CXX $CXX_FLAGS $CXX_INCL -c $SRC -o ${SRC%.cpp}.o
        if [ $? -ne 0 ]; then
            echo
            echo [-] $SRC : Failed to build an example ...aborted
            echo     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            clean_up_obj
            exit 1
        fi
    done

    OBJECTS=`find $EXAMPLE_DIR -name "*.o"`
    echo [*] Linking $OBJECTS ...
    $CXX $CXX_FLAGS $CXX_LIBS $OBJECTS -o $EXAMPLE_DIR/$EXEC_FILE
    if [ $? -ne 0 ]; then
        echo
        echo [-] $EXAMPLE_DIR : Failed to link object files ...aborted
        echo     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        clean_up_obj
        exit 1
    fi
done

clean_up_obj
echo [*] No compilation error detected
exit 0
