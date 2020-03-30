#!/bin/bash
#Readme: Execution: ./BuildAll.sh <JN5168 or JN5169>
echo "Building all the Light Bulbs ..."

rm -f BuildLog*_$1.txt
# 
if [ "$1" == "JN517x" ] || [ "$1" == "JN516x" ] ; then
    echo "Building ..."
    start=$(date +"%T")
    echo "Start Time : $start"
    
    if [ "$3" == "OTA" ]; then
        make LIGHT=ExtendedColorLight JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 DR=DR1175 TRACE=1 OTA=1 OTA_ENCRYPTED=0   clean all >> BuildLog_$2_ExtendedColorLight_OTA.txt &
        make LIGHT=ColorTemperatureLight JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 DR=DR1175 OTA=1 OTA_ENCRYPTED=0        clean all >> BuildLog_$2_ColorTemperatureLight_OTA.txt &
        make LIGHT=DimmableLight TRACE=1 JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 DR=DR1175 OTA=1 OTA_ENCRYPTED=0        clean all >> BuildLog_$2_DimmableLight_OTA.txt &
        echo "make clean all part-1 (OTA Targets) in progress, please wait!"
        wait
        
        make LIGHT=ExtendedColorLight JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 TRACE=1 DR=DR1175 OTA=1 OTA_ENCRYPTED=1   clean all >> BuildLog_$2_ExtendedColorLight_OTA_Enc.txt &
        make LIGHT=ColorTemperatureLight JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 OTA=1 DR=DR1175 OTA_ENCRYPTED=1        clean all >> BuildLog_$2_ColorTemperatureLight_OTA_Enc.txt &
        make LIGHT=DimmableLight TRACE=1 JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 OTA=1 DR=DR1175 OTA_ENCRYPTED=1        clean all >> BuildLog_$2_DimmableLight_OTA_Enc.txt &
        echo "make clean all part-2 (OTA Enc Targets) in progress, please wait!"
        wait
    else
        make LIGHT=ExtendedColorLight JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 TRACE=1 OTA=0 OTA_ENCRYPTED=0   clean all >> BuildLog_$2_ExtendedColorLight.txt &
        make LIGHT=ColorTemperatureLight JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 OTA=0 OTA_ENCRYPTED=0        clean all >> BuildLog_$2_ColorTemperatureLight.txt &
        make LIGHT=DimmableLight TRACE=1 JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2 OTA=0 OTA_ENCRYPTED=0        clean all >> BuildLog_$2_DimmableLight.txt &
        echo "make clean all part-1 (Individual Targets) in progress, please wait!"
        wait
        
        make LIGHT=ExtendedColorLight TRACE=1 OTA=0 GP_SUPPORT=1 JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2          clean all >> BuildLog_$2_ExtendedColorLight_GP.txt &
        make LIGHT=ColorTemperatureLight GP_SUPPORT=1 JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2                     clean all >> BuildLog_$2_ColorTemperatureLight_GP.txt &
        make LIGHT=DimmableLight GP_SUPPORT=1 JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2                             clean all >> BuildLog_$2_DimmableLight_GP.txt &   
        echo "make clean all part-2 (GP Targets) in progress, please wait!"
        wait
        
        make LIGHT=DimmableLight GP_SUPPORT=1 GP_DEVICE=PROXY_BASIC JENNIC_CHIP_FAMILY=$1 JENNIC_CHIP=$2       clean all >> BuildLog_$2_DimmableLight_GP_Proxy.txt &   
        echo "make clean all part-3 (GP Proxy Targets) in progress, please wait!"
        wait
    fi    
    end=$(date +"%T")
    echo "End Time : $end"
	echo "Done !!!"
else
    echo "Usage ./BuildAll.sh <Chip Type Family JN516x or 7x> <Chp Type JN5168 '69 '79> <OTA optional>"
fi
