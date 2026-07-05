#ifndef BOOT_H
#define BOOT_H

#include <stdbool.h>

/* Initialize Void OS Elite - displays boot sequence */
bool void_boot(void);

/* Display ASCII VOID logo */
void display_void_logo(void);

/* Display system information */
void display_system_info(void);

/* Display initialization progress */
void display_init_progress(const char *stage, int current, int total);

/* Display completion message */
void display_boot_complete(void);

#endif /* BOOT_H */

