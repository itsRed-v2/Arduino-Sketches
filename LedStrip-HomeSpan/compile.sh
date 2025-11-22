#!/bin/bash

for arg in "$@"
do
    if [ $arg = "-v" ]
    then
        VERBOSE="--verbose"
        echo ">> Compiling in verbose mode"
    elif [ $arg = "-u" ]
    then
        UPLOAD="--upload"
        echo ">> Uploading queued"
    elif [ $arg = "-m" ]
    then
        MONITOR=true
        echo ">> Monitor queued"
    elif [ $arg = "--skip-compile" ] || [ $arg = "-s" ]
    then
        SKIP_COMPILE=true
        echo ">> Skipping compile"
    fi
done

if [ ! $SKIP_COMPILE ]
then
    arduino-cli compile \
        -b esp32:esp32:esp32 \
        --build-property build.partitions=min_spiffs \
        --build-property upload.maximum_size=1966080 \
        --warnings all \
        --build-path ./build/ \
        $VERBOSE $UPLOAD \
        .
fi

if [ $SKIP_COMPILE ] && [ $UPLOAD ]
then
    echo ">> Manually uploading..."
    arduino-cli upload --build-path ./build/
fi

if [ $MONITOR ]
then
    echo ">> Starting serial monitor..."
    arduino-cli monitor -c 115200
fi
