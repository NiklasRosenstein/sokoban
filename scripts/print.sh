#!/bin/sh

BLACK="\033[0;30m"
RED="\033[0;31m"
GREEN="\033[0;32m"
ORANGE="\033[0;33m"
BLUE="\033[0;34m"
PURPLE="\033[0;35m"
CYAN="\033[0;36m"
LIGHTGRAY="\033[0;37m"

DARKGRAY="\033[1;30m"
LIGHTRED="\033[1;31m"
LIGHTGREEN="\033[1;32m"
YELLOW="\033[1;33m"
LIGHTBLUE="\033[1;34m"
LIGHTPURPLE="\033[1;35m"
LIGHTCYAN="\033[1;36m"
WHITE="\033[1;37m"

NC="\033[0m"


STR="$@"


# If output is not a terminal dont add color
if [[ ! -t 1 ]]; then
  echo "$STR"
  exit
fi


if [[ $STR == " [LD]"* ]]; then
  echo -e -n " ${CYAN}[LD]${NC}"
  echo "${STR:5}"
elif [[ $STR == " [CC]"* ]]; then
  echo -e -n " ${GREEN}[CC]${NC}"
  echo "${STR:5}"
elif [[ $STR == " [GEN]"* ]]; then
  echo -e -n " ${ORANGE}[GEN]${NC}"
  echo "${STR:6}"
elif [[ $STR == " [CLEAN]"* ]]; then
  echo -e -n " ${RED}[CLEAN]${NC}"
  echo "${STR:8}"
else
  echo "$@"
fi

