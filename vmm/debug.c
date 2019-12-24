/*
 * Copyright (c) 2019 Jie Zheng
 * 
 *      A native debugger: I am tring to honor teh GDB syntax in my debugger
 */

#include <debug.h>
#include <stdio.h>
#include <string.h>
#include <util.h>

static void
print_hint(struct hart * hartptr)
{
    printf(ANSI_COLOR_GREEN"(zelda.risc-v.dbg: "ANSI_COLOR_CYAN"0x%x"
           ANSI_COLOR_GREEN") "ANSI_COLOR_RESET,
           hartptr->pc);
}

#define CMDLINE_SIZE 256
#define TOKEN_SIZE 64
#define MAX_CMD_TOKEN_PREFIX 8

struct cmd_registery_item {
    char * cmd_prefixs[MAX_CMD_TOKEN_PREFIX];
    int (*func)(struct hart * hartptr, int argc, char *argv[]);
};


static int
dump_registers_info(struct hart * hartptr, int argc, char *argv[])
{
    printf(ANSI_COLOR_MAGENTA"[breakpoint at 0x%x]:\n", hartptr->pc);
    dump_hart(hartptr);
    printf(ANSI_COLOR_RESET);
    return ACTION_CONTINUE;
}

static int
dump_breakpoints_info(struct hart * hartptr, int argc, char *argv[])
{
    dump_breakpoints();
    return ACTION_CONTINUE;
}

static int
debug_continue(struct hart * hartptr, int argc, char *argv[])
{

    return ACTION_STOP;
}

static struct cmd_registery_item cmds_items[] = {
    {
        .cmd_prefixs = {
            "info",
            "registers",
            NULL
        },
        .func = dump_registers_info
    },
    {
        .cmd_prefixs = {
            "info",
            "breakpoints",
            NULL
        },
        .func = dump_breakpoints_info
    },
    {
        .cmd_prefixs = {
            "continue",
            NULL
        },
        .func = debug_continue 
    },
    {
        .cmd_prefixs = {
            "break",
            NULL
        },
        .func = add_breakpoint_command
    }
};

static int nr_cmds_items = sizeof(cmds_items) / sizeof(cmds_items[0]);

static int
process_cmds_tokens(struct hart * hartptr, int argc, char *argv[])
{
    int item_index = 0;
    struct cmd_registery_item * pitem = NULL;
    struct cmd_registery_item * item_found = NULL;
    int lpm_counter = 0;

    for (; item_index < nr_cmds_items; item_index++) {
        pitem = &cmds_items[item_index];
 
        int idx = 0;
        char * ptr_token = NULL;
        int lpm_counter_tmp = 0;
        for (idx = 0; idx < MAX_CMD_TOKEN_PREFIX; idx++) {
            ptr_token = pitem->cmd_prefixs[idx];
            if (!ptr_token) {
                lpm_counter_tmp = idx;
                break;
            }

            if (idx >= argc) {
                break;
            }

            if (strcmp(ptr_token, argv[idx])) {
                break;
            }
        }
        if (!ptr_token && lpm_counter_tmp > 0 &&
            lpm_counter_tmp > lpm_counter) {
            item_found = pitem;
            lpm_counter = lpm_counter_tmp;
        }
    }

    if (item_found && lpm_counter > 0) {
        return item_found->func(hartptr, argc - lpm_counter, argv + lpm_counter);
    } else {
        // NO ACTION taken
    }
    return ACTION_CONTINUE;
}

void
enter_vmm_dbg_shell(struct hart * hartptr, int check_bps)
{
    ASSERT(hartptr->hart_magic == HART_MAGIC_WORD);
    if (check_bps && !is_address_breakpoint(hartptr->pc)) {
        // The address is not tracked, go on.
        return;
    }
    char cmdline[CMDLINE_SIZE];
    char * tokens[TOKEN_SIZE];
    int nr_token;

    static char token_buffer[TOKEN_SIZE][TOKEN_SIZE];
    static char * last_tokens[TOKEN_SIZE];
    static int last_nr_token = 0;
    while (!0) {
        nr_token = 0;
        memset(cmdline, 0x0, sizeof(cmdline));
        memset(tokens, 0x0, sizeof(tokens));
        print_hint(hartptr);
        fgets(cmdline, sizeof(cmdline) - 1, stdin);

        {// strip the trailing carriage return '\n'
            char * ptr = cmdline;
            for (; *ptr; ptr++) {
                if (*ptr == '\n') {
                    *ptr = '\0';
                    break;
                }
            }
        }

        {// split the tokens.
            char *ptr = strtok(cmdline, " ");
            do {
                if (!ptr) {
                    break;
                }
                tokens[nr_token] = ptr;
                nr_token += 1;
                if (nr_token == TOKEN_SIZE) {
                    break;
                }
            } while ((ptr = strtok(NULL, " ")));
        }
        
        int action;
        if (nr_token) {
            action = process_cmds_tokens(hartptr, nr_token, tokens);
            // MUST make a copy.
            int idx = 0;
            for (idx = 0; idx < nr_token; idx++) {
                strcpy(token_buffer[idx], tokens[idx]);
                last_tokens[idx] = token_buffer[idx];
            }
            last_nr_token = nr_token;
        } else {
            action = process_cmds_tokens(hartptr, last_nr_token, last_tokens);
        }

        if (action == ACTION_STOP) {
            break;
        } else if(action == ACTION_CONTINUE) {

        } else {
            __not_reach();            
        }
    }
}



