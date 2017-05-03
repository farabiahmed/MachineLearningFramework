# Run this script on NTFS partition as follows:
# bash unicodeTest.sh

#echo -e "\xE2\x98\xA0"
#echo -e "\xE2\x98\x91"

echo -e
echo -e "Random Initial State"
#echo -en "\033[42m"
#echo -en "00 ["
#echo -e "\033[0m"
echo -e "00 ["
echo -e "01 [ [01]"
echo -e "02 [ [02]"
echo -e "03 [ [03] [04]"
echo -e "04 [ [05]"
echo -e "05 [ "
echo -ee

echo -e
#echo -e "Action: STACK(Block:#01,Slot:#00)"
echo -e "Action: STACK(Block:#01)"
echo -e "00 [ [01]"
echo -e "01 [ [02]"
echo -e "02 [ "
echo -e "03 [ [03] [04]"
echo -e "04 [ [05]"
echo -e "05 [ "
echo -e



echo -e
#echo -e "Action: UNSTACK(Block:#04,Slot:#01)"
echo -e "Action: UNSTACK(Block:#04)"
echo -e "00 [ [01] [02]"
echo -e "01 [ [04]"
echo -e "02 [ "
echo -e "03 [ [03]"
echo -e "04 [ [05]"
echo -e "05 [ "
echo -e

echo -e
#echo -e "Action: STACK(Block:#03,Slot:#00)"
echo -e "Action: STACK(Block:#03)"
echo -e "00 [ [01] [02] [03]"
echo -e "01 [ [04]"
echo -e "02 [ "
echo -e "03 [ "
echo -e "04 [ [05]"
echo -e "05 [ "
echo -e

echo -e
#echo -e "Action: STACK(Block:#04,Slot:#00)"
echo -e "Action: STACK(Block:#04)"
echo -e "00 [ [01] [02] [03] [04]"
echo -e "01 [ "
echo -e "02 [ "
echo -e "03 [ "
echo -e "04 [ [05]"
echo -e "05 [ "
echo -e

echo -e
#echo -e "Action: STACK(Block:#05,Slot:#00)"
echo -e "Action: STACK(Block:#05)"
echo -e "00 [ [01] [02] [03] [04] [05]"
echo -e "01 [ "
echo -e "02 [ "
echo -e "03 [ "
echo -e "04 [ "
echo -e "05 [ "
echo -e

