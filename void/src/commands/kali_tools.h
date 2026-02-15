#ifndef KALI_TOOLS_H
#define KALI_TOOLS_H

#include "../../include/void.h"

/* Kali Top 10 Security Tools */

/* Tool information structure */
typedef struct {
    const char *name;
    const char *github_url;
    const char **dependencies;
    const char **binaries;
    const char *install_script;
    bool requires_root;
} kali_tool_t;

/* Tool installation functions */
int install_kali_tool(const char *tool_name, shell_context_t *ctx);
int check_kali_tool_installed(const char *tool_name);
char *get_kali_tool_path(const char *tool_name);
int install_all_kali_tools(shell_context_t *ctx);
int list_kali_tools(void);

/* Individual tool installers */
int install_metasploit(shell_context_t *ctx);
int install_sqlmap(shell_context_t *ctx);
int install_nikto(shell_context_t *ctx);
int install_nmap(shell_context_t *ctx);
int install_wpscan(shell_context_t *ctx);
int install_gobuster(shell_context_t *ctx);
int install_feroxbuster(shell_context_t *ctx);
int install_john(shell_context_t *ctx);
int install_hashcat(shell_context_t *ctx);
int install_hydra(shell_context_t *ctx);

/* Tool execution helpers */
int execute_kali_tool(const char *tool_name, int argc, char **argv, shell_context_t *ctx);
char *find_tool_binary(const char *tool_name);

/* Tool catalog */
extern kali_tool_t kali_tools_catalog[];

#endif /* KALI_TOOLS_H */

