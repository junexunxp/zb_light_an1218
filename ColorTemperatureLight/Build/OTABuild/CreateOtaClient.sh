#!/bin/bash

echo "Creating OTA Client binaries..."
# $1 : the OTA build directory
# $2 : the SDK directory name
# $3 : the manufacturer code
# $4 : 32 byte OTA header string
# $5 : JET VERSION 4 - JN5169 and 5 for JN5179
# $6 : Jennic chip family like JN516x and JN517x
# $7 : OTA Device Id

JET=../../../../../sdk/$2/Tools/OTALinuxUtils/JET

# Change the path to the OTA Build folder.
cd $1

# ####################################################################################################
# ###################################Build Unencrpted Client Binary ##################################################

# Add serialisation Data with ImageType = 0x0XXX - Indicates it is for Encrpted devices
echo "Step 1"
$JET -m combine -f Light.bin -x configOTA_$6_Cer_Keys_HA_Light.txt -v $5 -g 1 -k 0xffffffffffffffffffffffffffffffff -u $3 -t $7 -j $4

# Creat an Unencrpted Bootable Client with Version 1
echo "Step 2"
echo $JET -m otamerge --embed_hdr -c outputffffffffffffffff.bin -o Client.bin -v $5 -n 1 -u $3 -t $7 -j $4
$JET -m otamerge --embed_hdr -c outputffffffffffffffff.bin -o Client.bin -v $5 -n 1 -u $3 -t $7 -j $4

# ###################Build OTA Unencrypted Upgarde Image from the Bootable Client  #########################
# Modify Embedded Header to reflect version 2 
echo "Step 3"
$JET -m otamerge --embed_hdr -c Client.bin -o UpGradeImagewithOTAHeaderV2.bin -v $5 -n 2 -u $3 -t $7 -j $4

# Wrap the Image with OTA header with version 2
echo "Step 4"
$JET -m otamerge --ota -c UpGradeImagewithOTAHeaderV2.bin -o ClientUpGradeImagewithOTAHeaderV2.bin -v $5 -n 2 -u $3 -t $7 -j $4
echo "Step 5"
$JET -m otamerge --ota -c UpGradeImagewithOTAHeaderV2.bin -o ClientUpGradeImagewithOTAHeaderV2.ota -p 1 -v $5 -n 2 -u $3 -t $7 -j $4

# Modify Embedded Header to reflect version 3 
echo "Step 6"
$JET -m otamerge --embed_hdr -c Client.bin -o UpGradeImagewithOTAHeaderV3.bin -v $5 -n 3 -u $3 -t $7 j $4

# Wrap the Image with OTA header with version 3
echo "Step 7"
$JET -m otamerge --ota -c UpGradeImagewithOTAHeaderV3.bin -o ClientUpGradeImagewithOTAHeaderV3.bin -v $5 -n 3 -u $3 -t $7 -j $4
echo "Step 8"
$JET -m otamerge --ota -c UpGradeImagewithOTAHeaderV3.bin -o ClientUpGradeImagewithOTAHeaderV3.ota -p 1 -v $5 -n 3 -u $3 -t $7 -j $4

# ####################################################################################################
# #################################### Clean Up Imtermediate files##################################################

echo "Step 9"

# rm Light.bin 
rm output*.bin
rm UpGradeImagewithOTAHeader*.bin

chmod 777 Client.bin
chmod 777 ClientUpGradeImagewithOTAHeaderV2.bin
chmod 777 ClientUpGradeImagewithOTAHeaderV3.bin
