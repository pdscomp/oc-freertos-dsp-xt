# scripts/mkcmd.sh
#
# (c) Copyright 2013
# Allwinner Technology Co., Ltd. <www.allwinnertech.com>
# James Deng <csjamesdeng@allwinnertech.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# export importance variable
XtensaTools_version=(
"RI-2020.4-linux"
)

allconfig=(
LICHEE_XTENSATOOLS
LICHEE_KERNEL_DIR
)

ic_version=(
"r528"
)

function mk_error()
{
	echo -e "\033[47;31mERROR: $*\033[0m"
}

function mk_warn()
{
	echo -e "\033[47;34mWARN: $*\033[0m"
}

function mk_info()
{
	echo -e "\033[47;30mINFO: $*\033[0m"
}


# define importance variable
export LICHEE_BUILD_DIR=$(cd $(dirname $0) && pwd)
export LICHEE_TOP_DIR=$(cd $LICHEE_BUILD_DIR/.. && pwd)
export LICHEE_KERNEL_DIR=${LICHEE_TOP_DIR}/kernel
export LICHEE_CHIP_DIR=${LICHEE_TOP_DIR}/projects
export LICHEE_CHIP_CORE_DIR
export LICHEE_CHIP_OUT_DIR
export LICHEE_CHIP_ARCH_DIR
export LICHEE_PACK_DIR=${LICHEE_TOP_DIR}/pack
export LICHEE_CUR_PATH
export LICHEE_HOST_PATH

# XTENSA config
export LM_LICENSE_FILE="27000@192.168.204.44"
export XTENSA_TOOLS_DIR
export XTENSA_SYSTEM
export XTENSA_PATH

#eg. save_config "LICHEE_PLATFORM" "$LICHEE_PLATFORM" $BUILD_CONFIG
function save_config()
{
	local cfgkey=$1
	local cfgval=$2
	local cfgfile=$3
	local dir=$(dirname $cfgfile)
	[ ! -d $dir ] && mkdir -p $dir
	cfgval=$(echo -e "$cfgval" | sed -e 's/^\s\+//g' -e 's/\s\+$//g')
	if [ -f $cfgfile ] && [ -n "$(sed -n "/^\s*export\s\+$cfgkey\s*=/p" $cfgfile)" ]; then
		sed -i "s|^\s*export\s\+$cfgkey\s*=\s*.*$|export $cfgkey=$cfgval|g" $cfgfile
	else
		echo "export $cfgkey=$cfgval" >> $cfgfile
	fi
}

function load_config()
{
	local cfgkey=$1
	local cfgfile=$2
	local defval=$3
	local val=""

	[ -f "$cfgfile" ] && val="$(sed -n "/^\s*export\s\+$cfgkey\s*=/h;\${x;p}" $cfgfile | sed -e 's/^[^=]\+=//g' -e 's/^\s\+//g' -e 's/\s\+$//g')"
	eval echo "${val:-"$defval"}"
}


function list_subdir()
{
	echo "$(eval "$(echo "$(ls -d $1/*/)" | sed  "s/^/basename /g")")"
}

function mk_select()
{
	local val_list=$1
	local cfg_key=$2
	local cnt=0
	local cfg_val=$(load_config $cfg_key $BUILD_CONFIG)
	local cfg_idx=0
	local banner=$(echo ${cfg_key:7} | tr '[:upper:]' '[:lower:]')

	printf "All available $banner:\n"
	for val in $val_list; do
		if [ ${val} != "config" ] ; then
			array[$cnt]=$val
			if [ "X_$cfg_val" == "X_${array[$cnt]}" ]; then
				cfg_idx=$cnt
			fi
			printf "%4d. %s\n" $cnt $val
			let "cnt++"
		fi
	done
	while true; do
		read -p "Choice [${array[$cfg_idx]}]: " choice
		if [ -z "${choice}" ]; then
			choice=$cfg_idx
		fi

		if [ -z "${choice//[0-9]/}" ] ; then
			if [ $choice -ge 0 -a $choice -lt $cnt ] ; then
				cfg_val="${array[$choice]}"
				break;
			fi
		fi
		 printf "Invalid input ...\n"
	done
	export $cfg_key=$cfg_val
	save_config "$cfg_key" "$cfg_val" $BUILD_CONFIG
}

function select_XtensaTools()
{
	local val_list="${XtensaTools_version[@]}"
	local cfg_key="LICHEE_XTENSATOOLS"
	mk_select "$val_list" "$cfg_key"

	#set xtensa tool env
	XTENSA_TOOLS_DIR=/opt/${LICHEE_XTENSATOOLS}/XtensaTools
	XTENSA_PATH=${XTENSA_TOOLS_DIR}/bin:${XTENSA_TOOLS_DIR}/lib/iss
	XTENSA_SYSTEM=${LICHEE_TOP_DIR}/XtDevTools/${LICHEE_XTENSATOOLS}/config
	LICHEE_HOST_PATH=${PATH}
	LICHEE_CUR_PATH=${PATH}:${XTENSA_PATH}

	save_config "XTENSA_TOOLS_DIR" "$XTENSA_TOOLS_DIR" $BUILD_CONFIG
	save_config "XTENSA_PATH" "$XTENSA_PATH" $BUILD_CONFIG
	save_config "XTENSA_SYSTEM" "$XTENSA_SYSTEM" $BUILD_CONFIG
	save_config "LICHEE_CUR_PATH" "$LICHEE_CUR_PATH" $BUILD_CONFIG
	save_config "LICHEE_HOST_PATH" "$LICHEE_HOST_PATH" $BUILD_CONFIG
}

function select_XtensaCore()
{
	local val_list=$(list_subdir XtDevTools/${LICHEE_XTENSATOOLS})
	local cfg_key="XTENSA_CORE"
	mk_select "$val_list" "$cfg_key"
}

function select_kernel()
{
	local val_list=$(list_subdir $LICHEE_KERNEL_DIR)
	local cfg_key="LICHEE_KERNEL"
	mk_select "$val_list" "$cfg_key"
}

function select_ic()
{
	local val_list="${ic_version[@]}"
	local cfg_key="LICHEE_IC"
	local arch_key
	mk_select "$val_list" "$cfg_key"
	LICHEE_CHIP_CORE_DIR=${LICHEE_CHIP_DIR}/${LICHEE_IC}
	save_config "LICHEE_CHIP_CORE_DIR" "$LICHEE_CHIP_CORE_DIR" $BUILD_CONFIG

	if [ "x$LICHEE_IC" == "xr528" ]; then
		arch_key="sun8iw20"
	else
		echo "ERROR: unkwon arch ..."
		return 1;
	fi
	save_config "LICHEE_CHIP_ARCH" "$arch_key" $BUILD_CONFIG

}

function select_DspCore()
{
	local val_list=$(list_subdir $LICHEE_CHIP_CORE_DIR)
	local cfg_key="LICHEE_DSP_CORE"
	mk_select "$val_list" "$cfg_key"
}

function mk_config()
{

	save_config "LICHEE_TOP_DIR" "$LICHEE_TOP_DIR" $BUILD_CONFIG

	select_XtensaTools
	select_XtensaCore
	select_kernel
	select_ic
	select_DspCore
	mktools

	LICHEE_CHIP_OUT_DIR=${LICHEE_TOP_DIR}/out/${LICHEE_IC}
	save_config "LICHEE_CHIP_OUT_DIR" "$LICHEE_CHIP_OUT_DIR" $BUILD_CONFIG

	LICHEE_CHIP_ARCH_DIR=${LICHEE_TOP_DIR}/arch/${LICHEE_CHIP_ARCH}
	save_config "LICHEE_CHIP_ARCH_DIR" "$LICHEE_CHIP_ARCH_DIR" $BUILD_CONFIG

	LICHEE_CHIP_LSP_DIR=${LICHEE_TOP_DIR}/arch/${LICHEE_CHIP_ARCH}/lsp/${LICHEE_DSP_CORE}
	save_config "LICHEE_CHIP_LSP_DIR" "$LICHEE_CHIP_LSP_DIR" $BUILD_CONFIG

}

function mktools()
{
	mk_info "Prepare executive of tools ..."
	. ${LICHEE_PACK_DIR}/build.sh
}

function mklichee()
{
	#set 
	XTENSA_PATH=${XTENSA_TOOLS_DIR}/bin:${XTENSA_TOOLS_DIR}/lib/iss
	XTENSA_SYSTEM=${LICHEE_TOP_DIR}/XtDevTools/${LICHEE_XTENSATOOLS}/config

	export PATH=${LICHEE_CUR_PATH}

	cp ${LICHEE_CHIP_CORE_DIR}/${LICHEE_DSP_CORE}/defconfig .config
	cd ${LICHEE_TOP_DIR} && make clean && make -j32 && make pack
}
