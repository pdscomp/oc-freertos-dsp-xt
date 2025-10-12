# OpenCentauri FreeRTOS DSP Xt version

FreeRTOS for r528, forked from official auto-t113-linux-V1.0 SDK release.

## How to Build

Create Xtensa install root:
```shell
cd freertos
LICHEE_TOP_DIR=/opt
LICHEE_XTENSATOOLS=$LICHEE_TOP_DIR/RI-2020.4-linux
XTENSA_SYSTEM=$LICHEE_XTENSATOOLS/XtensaTools/config
sudo mkdir -p $LICHEE_XTENSATOOLS
sudo chown -Rf $(id -u):$(id -g) $LICHEE_XTENSATOOLS
mkdir -p $LICHEE_XTENSATOOLS/XtensaTools/Tools/lic
```

Set up the configuration-independent Xtensa Tool:
```shell
tar zxvf XtDevTools/downloads/XtensaTools_RI_2020_4_linux.tgz -C $LICHEE_TOP_DIR
```

Set up the configuration-specific core files:
```shell
tar zxvf XtDevTools/downloads/aw_axi_cfg0_linux.tgz -C $LICHEE_TOP_DIR
```

Install a license for Xtensa tools (need XT_XCC_TIE entitlement):
```/opt/RI-2020.4-linux/XtensaTools/Tools/lic/license.dat```

Install the Xtensa development toolchain:
```shell
$LICHEE_XTENSATOOLS/aw_axi_cfg0/install \
  --xtensa-tools $LICHEE_XTENSATOOLS/XtensaTools \
  --registry $XTENSA_SYSTEM
```

Configure the build:
```shell
./build.sh config
```

You will be prompted with the following questions:
```
Welcome to mkscript setup progress
All available xtensatools:
   0. RI-2020.4-linux
Choice [RI-2020.4-linux]:
All available core:
   0. XtensaTools
   1. aw_axi_cfg0
Choice [XtensaTools]: 1
All available kernel:
   0. FreeRTOS_xtensa_v1.7
Choice [FreeRTOS_xtensa_v1.7]:
All available ic:
   0. r528
Choice [r528]:
All available dsp_core:
   0. dsp0
Choice [dsp0]:
INFO: Prepare executive of tools ...
```

Then, update the build configuration:
```shell
sed -re 's|^(.*XTENSA_SYSTEM)=.*$|\1=/opt/RI-2020.4-linux/XtensaTools/config|' -i .buildconfig
```

Set up the environment:
```shell
. build/envsetup.sh
```

The output will be:
```
Invoke . build/envsetup.sh from your shell to add the following functions to your environment:
    croot               - Changes directory to the top of the tree.
    cproject            - Changes directory to the project
    carch               - Changes directory to the arch
    clsp                - Changes directory to the lsp
    cout                - Changes directory to the out
Invoke . build/envsetup.sh from your shell to add the following functions to your environment:
    env_xcc             - use xcc env
    env_host            - use host env
run envsetup finish.
```

Activate the cross-compiler environment:
```shell
env_xcc
```

Finally, build the project:
```shell
make
```

## License

License for the included FreeRTOS is MIT, scripts and orchestration from Allwinner appears to be GPLv2 based on comments in script and header files.
