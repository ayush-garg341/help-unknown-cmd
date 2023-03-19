#### Simplified version of git - help unknown command.
#### When we type git "command" and command is wrong, git suggests similar command.
#### This is simplified implementation of the same for learning purpose only.
#### To run the code
    - git clone <repo>
    - cd help-unknown-cmd
    - Compile it with gcc -o help.out help_unknown_cmd/help.c help_unknown_cmd/exec-cmd.c help_unknown_cmd/stable-qsort.c
    - ./help.out git_command
        - example: ./help.out sta
                    ./help.out sttus
