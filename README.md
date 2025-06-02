# Memory Scanner
This simple CLI-Application provides basic scanning and manipulation of other processes memory.

## Overview
### Following commands exist:
`tasklist` To print all running tasks + pid\
`search` initiates a search, prompting for the target process and the value size.
         Afterwards search-values can be entered.\
`save` Only possible when a search is running, saves current match scope. This does not permanently save, just while the programm is running.\
`write` 1) Write to saved matches or 2) write to all matches of current search.\
`help` Print help\
`inject` 1) Absolute Path to your DLL file 2) PID of the Process to inject to.\

## Installation/Setup
For now .sln can be used. Will be changed to a better system later.
