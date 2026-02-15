#include "../include/void.h"
#include "core/boot.h"
#include "core/shell.h"
#include "utils/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    /* Initialize logger */
    if (!logger_init()) {
        fprintf(stderr, "Failed to initialize logger\n");
        return 1;
    }
    
    LOG_INFO("Void OS Elite starting...");
    
    /* Display boot sequence */
    if (!void_boot()) {
        LOG_ERROR("Boot sequence failed");
        logger_cleanup();
        return 1;
    }
    
    /* Initialize shell */
    shell_context_t *ctx = shell_init();
    if (ctx == NULL) {
        LOG_ERROR("Failed to initialize shell");
        logger_cleanup();
        return 1;
    }
    
    /* Run main shell loop */
    shell_loop(ctx);
    
    /* Cleanup */
    shell_cleanup(ctx);
    logger_cleanup();
    
    LOG_INFO("Void OS Elite shutting down");
    
    return 0;
}

