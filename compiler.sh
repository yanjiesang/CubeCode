#!/bin/bash

#################################################################################################################################
#目录设置
curr_dir=$(cd $(dirname $0); pwd)                           #当前路径
root_dir=${curr_dir}                                        #跟路径
build_dir=None                                              #编译目录
webrtc_dir=${root_dir}/AwsKVSWebRTC/open-source             #aws-webrtc目录
compile_script_file_path=${root_dir}/script/compile         #第三方库编译脚本目录

#交叉编译工具链设置
toolchainfile_cmake=${root_dir}/Toolchainfile.cmake         #交叉编译工具链CMAKE配置文件(勿修改)
toolchain=prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin      #交叉编译工具链路径，只需修改这里的路径，无需修改CMAKE配置文件中的路径
toolchain_path=${root_dir}/${toolchain}                     #工具链完整路径(勿修改)
cross_toolchain=aarch64-linux-gnu-                          #交叉编译链
cross_toolchain_host=aarch64-linux-gnu                      #交叉编译主机

#编译选项
EnableCrossCompile=$1                                       #使能交叉编译

#应用程序参数
SoftwareReleaseVersionFormat="1.0.0"                        #设置发布版本的格式
SoftwareDebugVersionFormat="1.0.0.1"                        #设置调试版本的格式
SoftwareReleaseVersion=$2                                   #应用程序发布软件版本
SoftwareDebugVersion=$3                                     #应用程序调试软件版本
SoftwareLogout="ON"                                         #日志打印开关
SoftwarePlatform="Default"                                  #应用程序应用平台(勿修改)

#################################################################################################################################

complier_help(){
    if [ $1 == 0 ];then
        echo "================================ complier.sh Help ====================================="
        echo " Function Description : "
        echo "      This shell was designed for complier this project easily"
        echo 
        echo " Parameter Description :"
        echo "      parma1: For Enable CrossCompile, should set ON/OFF" 
        echo "      parma2: For Software Release Version"
        echo "      parma3: For Software Debug Version"
        echo
        echo " Example Usage: "
        echo "      $0 ON 1.0.0 1.0.0.1"
        echo 
        echo "Then you should complier this project follow this help! ^_^"
        echo "================================== Copyright 2020 ====================================="
        exit;
    fi
}

compile_env_check(){

    # 交叉编译选择
    if [ -z ${EnableCrossCompile} ];then
        echo "Please confirm whether use Cross Compiler(Second Parameter should set ON/OFF, ON indicata use Cross Compiler)"
        exit 1
    else
        if [ "${EnableCrossCompile}" == "ON" ];then
            if [ ! -d ${root_dir}/build_arm ];then
                mkdir ${root_dir}/build_arm
            fi
            build_dir=${root_dir}/build_arm

            SoftwarePlatform="SOFTWARE_PLATFORM_ARM"
            echo "EnableCrossCompile=${EnableCrossCompile}"
            export WEBRTC_ENABLE_CROSS_COMPILE="ON"
            export WEBRTC_TOOLCHAINFILE_CMAKE=${toolchainfile_cmake}
            export WEBRTC_TOOLCHAIN=${toolchain}
            export WEBRTC_TOOLCHAIN_PATH=${toolchain_path}
            export WEBRTC_CROSS_TOOLCHAIN=${cross_toolchain}
            export WEBRTC_CROSS_TOOLCHAIN_HOST=${cross_toolchain_host}

            cp ${root_dir}/script/Toolchainfile.cmake ${root_dir}

            # 修改Toolchainfile.cmake 
            if [ ! -z $(grep -n "TOOLCHAIN_DIR \${CMAKE_CURRENT_SOURCE_DIR}" ${toolchainfile_cmake} | awk -F ':' '{print $1}') ];then
                find_line_index=$(grep -n "TOOLCHAIN_DIR \${CMAKE_CURRENT_SOURCE_DIR}" ${toolchainfile_cmake} | awk -F ':' '{print $1}')
                sed -i "${find_line_index}d" ${toolchainfile_cmake}
                find_line_index=$((${find_line_index}-1))
                sed -i "${find_line_index}a\SET(TOOLCHAIN_DIR \${CMAKE_CURRENT_SOURCE_DIR}\/${toolchain})" ${toolchainfile_cmake}
            fi

            # 修改Toolchainfile.cmake 
            if [ ! -z $(grep -n "SET(TARGET_CROSS" ${toolchainfile_cmake} | awk -F ':' '{print $1}') ];then
                find_line_index=$(grep -n "SET(TARGET_CROSS" ${toolchainfile_cmake} | awk -F ':' '{print $1}')
                sed -i "${find_line_index}d" ${toolchainfile_cmake}
                find_line_index=$((${find_line_index}-1))
                sed -i "${find_line_index}a\SET(TARGET_CROSS ${cross_toolchain})" ${toolchainfile_cmake}
            fi

        elif [ "${EnableCrossCompile}" == "OFF" ];then
            if [ ! -d ${root_dir}/build_x86 ];then
                mkdir ${root_dir}/build_x86
            fi
            build_dir=${root_dir}/build_x86

            echo "EnableCrossCompile=${EnableCrossCompile}"
            SoftwarePlatform="SOFTWARE_PLATFORM_X86" 
            export WEBRTC_ENABLE_CROSS_COMPILE="OFF"
        else
            echo "Error: Second Parameter should set ON/OFF, ON indicata use Cross Compiler"
            exit 1
        fi 
    fi

    if [ -z ${SoftwareReleaseVersion} ];then
        echo "Error: Please Input Software Release Version like ${SoftwareReleaseVersionFormat}"
        exit 1
    else
        if [ ${#SoftwareReleaseVersion} -ne ${#SoftwareReleaseVersionFormat} ];then
            echo "Error: Please Input Software Release Version like ${SoftwareReleaseVersionFormat}"
            exit 1
        else
            echo "SoftwareReleaseVersion=${SoftwareReleaseVersion}"
        fi 
    fi

    if [ -z ${SoftwareDebugVersion} ];then
        echo "Error: Please Input Software Debug Version like ${SoftwareDebugVersionFormat}"
        exit 1
    else
        if [ ${#SoftwareDebugVersion} -ne ${#SoftwareDebugVersionFormat} ];then
            echo "Error: Please Input Software Debug Version like ${SoftwareDebugVersionFormat}"
            exit 1
        else 
            echo "SoftwareDebugVersion=${SoftwareDebugVersion}" 
        fi 
    fi
}

complier(){
    compile_env_check

    echo "=================================================================================================="
    echo "                                     Complier Start                                               "
    echo "=================================================================================================="

    cd ${build_dir}
    rm -rf *

    if [ "${EnableCrossCompile}" == "ON" ];then
        if [ "${SoftwareLogout}" == "ON" ];then
            cmake ${root_dir} -DCMAKE_TOOLCHAIN_FILE=${toolchainfile_cmake} -DSOFTWARE_PLATFORM=${SoftwarePlatform} -DSOFTWARE_RELEASE_VERSION=${SoftwareReleaseVersion} -DSOFTWARE_DEBUG_VERSION=${SoftwareDebugVersion} -DSOFTWARE_LOGOUT_SWITCH=${SoftwareLogout}
        else
            cmake ${root_dir} -DCMAKE_TOOLCHAIN_FILE=${toolchainfile_cmake} -DSOFTWARE_PLATFORM=${SoftwarePlatform} -DSOFTWARE_RELEASE_VERSION=${SoftwareReleaseVersion} -DSOFTWARE_DEBUG_VERSION=${SoftwareDebugVersion}
        fi
    else
        if [ "${SoftwareLogout}" == "ON" ];then 
            cmake ${root_dir} -DSOFTWARE_PLATFORM=${SoftwarePlatform} -DSOFTWARE_RELEASE_VERSION=${SoftwareReleaseVersion} -DSOFTWARE_DEBUG_VERSION=${SoftwareDebugVersion} -DSOFTWARE_LOGOUT_SWITCH=${SoftwareLogout}
        else
            cmake ${root_dir} -DSOFTWARE_PLATFORM=${SoftwarePlatform} -DSOFTWARE_RELEASE_VERSION=${SoftwareReleaseVersion} -DSOFTWARE_DEBUG_VERSION=${SoftwareDebugVersion}
        fi
    fi

    make -j4

    cd ${root_dir}
    rm -rf ${toolchainfile_cmake}

    source ${root_dir}/script/copy.sh

    echo "=================================================================================================="
    echo "                                     Complier Finish                                              "
    echo "=================================================================================================="
}

complier_help $#
complier

exit 0
