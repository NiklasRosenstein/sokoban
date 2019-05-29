#!/bin/bash

NAME="Sokoban"


tmux has-session -t "$NAME"
if [[ $? -eq 0 ]]; then
  tmux attach-session -t "$NAME"
  exit 0
fi

tmux new-session -s "$NAME" -A -d

tmux rename-window 'code'
tmux split-window -v -p 15
tmux new-window -n 'make'
tmux new-window -n 'tags'

tmux select-window -t 1
tmux select-pane -t 1

tmux attach-session -t "$NAME"
