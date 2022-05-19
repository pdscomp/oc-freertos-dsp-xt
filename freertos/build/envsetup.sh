#!/bin/bash

function xcc_help()
{
    printf "Invoke . build/envsetup.sh from your shell to add the following functions to your environment:
    env_xcc             - use xcc env
    env_host            - use host env
"
    return 0
}

function build_help()
{
    printf "Invoke . build/envsetup.sh from your shell to add the following functions to your environment:
    croot               - Changes directory to the top of the tree.
    cproject            - Changes directory to the project
    carch               - Changes directory to the arch
    clsp            	- Changes directory to the lsp
    cout                - Changes directory to the out
"
    return 0
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


function printconfig()
{
	cat $LICHEE_TOP_DIR/.buildconfig
}

function env_xcc()
{
	local dkey="LICHEE_TOP_DIR"
	local dval=$(load_config $dkey $LICHEE_TOP_DIR/.buildconfig)
	[ -z "$dval" ] && echo "ERROR: $dkey not set in .buildconfig" && return 1
	PATH=${LICHEE_CUR_PATH}
}

function env_host()
{
	local dkey="LICHEE_HOST_PATH"
	local dval=$(load_config $dkey $LICHEE_TOP_DIR/.buildconfig)
	[ -z "$dval" ] && echo "ERROR: $dkey not set in .buildconfig" && return 1
	PATH=${LICHEE_HOST_PATH}
}

function croot()
{
	local dkey="LICHEE_TOP_DIR"
	local dval=$(load_config $dkey $LICHEE_TOP_DIR/.buildconfig)
	[ -z "$dval" ] && echo "ERROR: $dkey not set in .buildconfig" && return 1
	cd $dval
}

function cout()
{
	local dkey="LICHEE_CHIP_OUT_DIR"
	local dval=$(load_config $dkey $LICHEE_TOP_DIR/.buildconfig)
	[ -z "$dval" ] && echo "ERROR: $dkey not set in .buildconfig" && return 1
	cd $dval
}

function cproject()
{
	local dkey="LICHEE_CHIP_CORE_DIR"
	local dval_0=$(load_config $dkey $LICHEE_TOP_DIR/.buildconfig)
	[ -z "$dval_0" ] && echo "ERROR: $dkey not set in .buildconfig" && return 1

	dkey="LICHEE_DSP_CORE"
	local dval_1=$(load_config $dkey $LICHEE_TOP_DIR/.buildconfig)
	[ -z "$dval_1" ] && echo "ERROR: $dkey not set in .buildconfig" && return 1
	cd $dval_0/$dval_1/src
}

function carch()
{
	local dkey="LICHEE_CHIP_ARCH_DIR"
	local dval=$(load_config $dkey $LICHEE_TOP_DIR/.buildconfig)
	[ -z "$dval" ] && echo "ERROR: $dkey not set in .buildconfig" && return 1
	cd $dval
}

function clsp()
{
	local dkey="LICHEE_CHIP_LSP_DIR"
	local dval=$(load_config $dkey $LICHEE_TOP_DIR/.buildconfig)
	[ -z "$dval" ] && echo "ERROR: $dkey not set in .buildconfig" && return 1
	cd $dval
}

if [ ! -f build/envsetup.sh ] || [ ! -f build.sh ]; then
	echo "MUST do this in LICHEE_TOP_DIR."
else
    if [ ! -f .buildconfig ]; then
	./build.sh config
    fi

	export LICHEE_TOP_DIR=$(pwd)
    . $LICHEE_TOP_DIR/.buildconfig
	build_help
	xcc_help
    echo "run envsetup finish."
fi
