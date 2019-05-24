#!/bin/sh

# This script is supposed to be POSIX compatible.
# Check with ShellCheck or at https://www.shellcheck.net

#BLACK="\033[0;30m"
RED="\033[0;31m"
GREEN="\033[0;32m"
ORANGE="\033[0;33m"
#BLUE="\033[0;34m"
#PURPLE="\033[0;35m"
CYAN="\033[0;36m"
#LIGHTGRAY="\033[0;37m"

#DARKGRAY="\033[1;30m"
#LIGHTRED="\033[1;31m"
#LIGHTGREEN="\033[1;32m"
#YELLOW="\033[1;33m"
#LIGHTBLUE="\033[1;34m"
#LIGHTPURPLE="\033[1;35m"
#LIGHTCYAN="\033[1;36m"
#WHITE="\033[1;37m"

NC="\033[0m"


STR="$*"


# If output is not a terminal dont add color
if [ ! -t 1 ]; then
  echo "$STR"
  exit
fi

case "$STR" in
" [LD]"*)
    STR=$(echo "$STR" | cut -c 6-)
    #printf "${CYAN}"
    #printf " [LD]"
    #printf "${NC}"
    #printf "%s\n" "$STR"
    printf " ${CYAN}[LD]${NC}%s\n" "$STR"
    ;;
" [CC]"*)
    STR=$(echo "$STR" | cut -c 6-)
    #printf "${GREEN}"
    #printf " [CC]"
    #printf "${NC}"
    #printf "%s\n" "$STR"
    printf " ${GREEN}[CC]${NC}%s\n" "$STR"
    ;;
" [GEN]"*)
    STR=$(echo "$STR" | cut -c 7-)
    #printf "${ORANGE}"
    #printf " [GEN]"
    #printf "${NC}"
    #printf "%s\n" "$STR"
    printf " ${ORANGE}[GEN]${NC}%s\n" "$STR"
    ;;
" [CLEAN]"*)
    STR=$(echo "$STR" | cut -c 9-)
    #printf "$RED"
    #printf " [CLEAN]"
    #printf "$NC"
    #printf "%s\n" "$STR"
    printf " ${RED}[CLEAN]${NC}%s\n" "$STR"
    ;;
*)
    printf "%s\n" "$STR"
    ;;
esac

