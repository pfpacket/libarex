#!/bin/sh
#
#       example/build_test.sh
#   A simple script for building examples and tests
#
AREX_ROOT=`pwd`
BOOST_ROOT=~/boost/
CXX=g++
CXX_FLAGS="-std=c++0x -Wall"
EXEC_FILE=example.out
NO_REMOVE_OBJ=0
EXEC_TEST_SUITE=1
EXAMPLE_DIR_LIST=""

usage()
{
    echo "Usage: build_test.sh [options]"
    echo "Options:"
    echo "   -a arg     specify libarex root path (default: ./)"
    echo "   -b arg     specify Boost root path   (default: ~/boost/)"
    echo "   -c arg     specify C++ compiler      (default: g++)"
    echo "   -d arg     set target directory as arg"
    echo "   -f arg     add additional flags passed to compiler"
    echo "   -l         clean up object files"
    echo "   -n         never remove object files"
    echo "   -u         do not test"
    echo "   -h         print this help and exit"
}

clean_up_obj()
{
    if [ $NO_REMOVE_OBJ -eq 0 ]; then
        find $AREX_ROOT/example/ -name "*.o" -exec rm -f {} \;
        find $AREX_ROOT/example/ -name $EXEC_FILE -exec rm -f {} \;
    fi
}

signal_handler()
{
    clean_up_obj
    echo
    echo [!] Keyboard interrupt occurred \(SIGINT\)
}

is_leaf_dir()
{
    RET=1
    [ "`find "$1" -maxdepth 1 -type d | wc -l`" = "1" ] && RET=0
    return $RET
}

die()
{
    clean_up_obj
    echo [-] Fatal Error occurred: Abort
    exit 1
}

start_test()
{
    for TEST_DIR in `find $AREX_ROOT/example/test/ -type d`
    do
        is_leaf_dir $TEST_DIR || continue
        $TEST_DIR/$EXEC_FILE  || die
    done
}

while getopts a:b:c:d:f:lnuh OPTION
do
    case $OPTION in
        a)  AREX_ROOT=$OPTARG   ;;
        b)  BOOST_ROOT=$OPTARG  ;;
        c)  CXX=$OPTARG ;;
        d)  EXAMPLE_DIR_LIST=$OPTARG
            EXEC_TEST_SUITE=0   ;;
        f)  CXX_FLAGS="$CXX_FLAGS $OPTARG" ;;
        l)  clean_up_obj
            exit 0 ;;
        n)  NO_REMOVE_OBJ=1     ;;
        u)  EXEC_TEST_SUITE=0   ;;
        h)  usage
            exit 0 ;;
        \?) usage
            exit 1 ;;
    esac
done

CXX_INCL="-I $BOOST_ROOT/include/ -I $AREX_ROOT/include"
CXX_LIBS="-L $BOOST_ROOT/lib/ -lpthread -lboost_system"
if [ "${EXAMPLE_DIR_LIST}" = "" ]; then
    EXAMPLE_DIR_LIST=`find $AREX_ROOT/example/* -type d`
fi
if [ $? -ne 0 ]; then
    echo
    echo [-] Failed to find the directory of example/
    echo [-] Test failed ...compiling aborted
    exit 1
fi

trap "signal_handler" INT
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
            echo [-] $SRC : Failed to build an example
            echo     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            die
        fi
    done

    OBJECTS=`find $EXAMPLE_DIR -name "*.o"`
    echo [*] Linking $OBJECTS ...
    $CXX $CXX_FLAGS $CXX_LIBS $OBJECTS -o $EXAMPLE_DIR/$EXEC_FILE
    if [ $? -ne 0 ]; then
        echo
        echo [-] $EXAMPLE_DIR : Failed to link object files
        echo     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        die
    fi
done
echo [*] No compilation error detected

if [ $EXEC_TEST_SUITE -eq 1 ]; then
    echo [*] Starting test ...
    start_test
    echo [*] Test code passed. No error detected
fi

clean_up_obj
exit 0
