#!/bin/bash

curr_dir=$(cd $(dirname $0);pwd)
root_dir=${curr_dir}

if [ ${WEBRTC_ENABLE_CROSS_COMPILE} == "ON" ];then
build_dir=${root_dir}/build_arm
else
build_dir=${root_dir}/build_x86
fi

cp -rf ${root_dir}/test/h264SampleFrames ${build_dir}
