#include "utils.h"
#include "../utils/logger.h"
#include "../utils/string.h"
#include "../utils/base64_native.h"
#include "../utils/hash_native.h"
#include "../../include/void.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <sys/random.h>

/* Native MD5 hash */
static char *hash_md5(const char *input) {
    if (input == NULL) return NULL;
    return hash_md5_native((const unsigned char *)input, strlen(input));
}

/* Native SHA256 hash */
static char *hash_sha256(const char *input) {
    if (input == NULL) return NULL;
    return hash_sha256_native((const unsigned char *)input, strlen(input));
}

int cmd_summon(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: summon <file>\n" COLOR_RESET);
        printf(COLOR_GREY "Note: Native script execution - supports C binaries, shell scripts\n" COLOR_RESET);
        return 1;
    }
    
    const char *file = argv[1];
    
    /* Check if file exists and is executable */
    struct stat st;
    if (stat(file, &st) != 0) {
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        return 1;
    }
    
    if (!(st.st_mode & S_IXUSR)) {
        printf(COLOR_GREY "File is not executable. Use 'incantation' to grant execute permission.\n" COLOR_RESET);
        return 1;
    }
    
    /* Execute file directly */
    pid_t pid = fork();
    if (pid < 0) {
        LOG_ERROR("Failed to fork: %s", strerror(errno));
        return 1;
    }
    
    if (pid == 0) {
        /* Child: execute the file */
        char *exec_argv[argc];
        exec_argv[0] = (char*)file;
        for (int i = 2; i < argc; i++) {
            exec_argv[i-1] = argv[i];
        }
        exec_argv[argc-1] = NULL;
        
        execv(file, exec_argv);
        LOG_ERROR("Failed to execute file: %s", strerror(errno));
        exit(1);
    } else {
        /* Parent: wait for child */
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return 1;
    }
}

int cmd_forgepy(int argc, char **argv, shell_context_t *ctx) {
    (void)argc;
    (void)argv;
    (void)ctx;
    
    printf(COLOR_GREY "Native Python interpreter not yet implemented.\n" COLOR_RESET);
    printf(COLOR_GREY "Void OS is working towards a native Python implementation.\n" COLOR_RESET);
    printf(COLOR_GREY "For now, Python scripts can be executed via 'summon' if Python is installed.\n" COLOR_RESET);
    return 1;
}

/* Native Base64 encode */
static char *base64_encode(const char *input) {
    if (input == NULL) return NULL;
    size_t output_len;
    return base64_encode_native((const unsigned char *)input, strlen(input), &output_len);
}

/* Native Base64 decode */
static char *base64_decode(const char *input) {
    if (input == NULL) return NULL;
    size_t output_len;
    unsigned char *decoded = base64_decode_native(input, strlen(input), &output_len);
    if (decoded == NULL) return NULL;
    
    /* Convert to null-terminated string */
    char *result = malloc(output_len + 1);
    if (result == NULL) {
        free(decoded);
        return NULL;
    }
    memcpy(result, decoded, output_len);
    result[output_len] = '\0';
    free(decoded);
    return result;
}

int multitool_hash(int argc, char **argv) {
    if (argc < 3) {
        printf(COLOR_GREY "Usage: multitool hash <md5|sha256> <input>\n" COLOR_RESET);
        return 1;
    }
    
    const char *algo = argv[1];
    const char *input = argv[2];
    
    char *hash = NULL;
    if (strcmp(algo, "md5") == 0) {
        hash = hash_md5(input);
    } else if (strcmp(algo, "sha256") == 0) {
        hash = hash_sha256(input);
    } else {
        printf(COLOR_GREY "Unknown algorithm. Use md5 or sha256.\n" COLOR_RESET);
        return 1;
    }
    
    if (hash != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, hash);
        free(hash);
        return 0;
    }
    
    printf(COLOR_GREY "Failed to generate hash.\n" COLOR_RESET);
    return 1;
}

int multitool_base64(int argc, char **argv) {
    if (argc < 3) {
        printf(COLOR_GREY "Usage: multitool base64 <encode|decode> <input>\n" COLOR_RESET);
        return 1;
    }
    
    const char *op = argv[1];
    const char *input = argv[2];
    
    char *result = NULL;
    if (strcmp(op, "encode") == 0) {
        result = base64_encode(input);
    } else if (strcmp(op, "decode") == 0) {
        result = base64_decode(input);
    } else {
        printf(COLOR_GREY "Unknown operation. Use encode or decode.\n" COLOR_RESET);
        return 1;
    }
    
    if (result != NULL) {
        printf(COLOR_WHITE "%s\n" COLOR_RESET, result);
        free(result);
        return 0;
    }
    
    printf(COLOR_GREY "Failed to process base64.\n" COLOR_RESET);
    return 1;
}

int multitool_json(int argc, char **argv) {
    if (argc < 2) {
        printf(COLOR_GREY "Usage: multitool json <input>\n" COLOR_RESET);
        printf(COLOR_GREY "Note: Basic JSON validation. For formatting, use external tools.\n" COLOR_RESET);
        return 1;
    }
    
    /* Basic JSON validation - check for balanced braces */
    const char *input = argv[1];
    int braces = 0;
    int brackets = 0;
    bool in_string = false;
    
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '"' && (i == 0 || input[i-1] != '\\')) {
            in_string = !in_string;
        } else if (!in_string) {
            if (input[i] == '{') braces++;
            else if (input[i] == '}') braces--;
            else if (input[i] == '[') brackets++;
            else if (input[i] == ']') brackets--;
        }
    }
    
    if (braces == 0 && brackets == 0 && !in_string) {
        printf(COLOR_GREEN "Valid JSON structure.\n" COLOR_RESET);
        return 0;
    } else {
        printf(COLOR_GREY "Invalid JSON structure.\n" COLOR_RESET);
        return 1;
    }
}

int multitool_uuid(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    /* Generate UUID v4 format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx */
    unsigned char uuid[16];
    
    #ifdef __linux__
    if (getrandom(uuid, 16, 0) != 16) {
        /* Fallback to /dev/urandom */
        FILE *urand = fopen("/dev/urandom", "r");
        if (urand != NULL) {
            fread(uuid, 1, 16, urand);
            fclose(urand);
        }
    }
    #else
    /* macOS/BSD - use arc4random or /dev/urandom */
    FILE *urand = fopen("/dev/urandom", "r");
    if (urand != NULL) {
        fread(uuid, 1, 16, urand);
        fclose(urand);
    }
    #endif
    
    /* Set version (4) and variant bits */
    uuid[6] = (uuid[6] & 0x0F) | 0x40; /* Version 4 */
    uuid[8] = (uuid[8] & 0x3F) | 0x80; /* Variant 10 */
    
    /* Format as UUID string */
    char uuid_str[37];
    snprintf(uuid_str, sizeof(uuid_str),
             "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid[0], uuid[1], uuid[2], uuid[3],
             uuid[4], uuid[5], uuid[6], uuid[7],
             uuid[8], uuid[9], uuid[10], uuid[11],
             uuid[12], uuid[13], uuid[14], uuid[15]);
    
    printf(COLOR_WHITE "%s\n" COLOR_RESET, uuid_str);
    return 0;
}

int multitool_timestamp(int argc, char **argv) {
    if (argc < 2) {
        /* Show current timestamp */
        time_t now = time(NULL);
        printf(COLOR_WHITE "Unix: %ld\n" COLOR_RESET, (long)now);
        
        struct tm *tm_info = localtime(&now);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
        printf(COLOR_WHITE "Local: %s\n" COLOR_RESET, time_str);
        return 0;
    }
    
    /* Convert timestamp */
    long timestamp = atol(argv[1]);
    time_t t = (time_t)timestamp;
    
    struct tm *tm_info = localtime(&t);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    printf(COLOR_WHITE "Unix: %ld\n" COLOR_RESET, timestamp);
    printf(COLOR_WHITE "Local: %s\n" COLOR_RESET, time_str);
    return 0;
}

int cmd_multitool(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_WHITE "Multitool - Developer Utility Panel\n" COLOR_RESET);
        printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
        printf(COLOR_WHITE "Available tools:\n" COLOR_RESET);
        printf(COLOR_GREY "  hash <md5|sha256> <input>     - Generate hash\n" COLOR_RESET);
        printf(COLOR_GREY "  base64 <encode|decode> <input> - Base64 operations\n" COLOR_RESET);
        printf(COLOR_GREY "  json <input>                   - JSON validation\n" COLOR_RESET);
        printf(COLOR_GREY "  uuid                           - Generate UUID\n" COLOR_RESET);
        printf(COLOR_GREY "  timestamp [unix]               - Timestamp converter\n" COLOR_RESET);
        return 0;
    }
    
    const char *tool = argv[1];
    
    if (strcmp(tool, "hash") == 0) {
        return multitool_hash(argc - 1, &argv[1]);
    } else if (strcmp(tool, "base64") == 0) {
        return multitool_base64(argc - 1, &argv[1]);
    } else if (strcmp(tool, "json") == 0) {
        return multitool_json(argc - 1, &argv[1]);
    } else if (strcmp(tool, "uuid") == 0) {
        return multitool_uuid(argc - 1, &argv[1]);
    } else if (strcmp(tool, "timestamp") == 0) {
        return multitool_timestamp(argc - 1, &argv[1]);
    } else {
        printf(COLOR_GREY "Unknown tool. Run 'multitool' for help.\n" COLOR_RESET);
        return 1;
    }
}

int cmd_fabricate(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: fabricate <name> [type]\n" COLOR_RESET);
        printf(COLOR_GREY "Types: c, py, js\n" COLOR_RESET);
        return 1;
    }
    
    const char *name = argv[1];
    const char *type = (argc > 2) ? argv[2] : "c";
    
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.%s", name, type);
    
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        LOG_ERROR("Failed to create file '%s': %s", filename, strerror(errno));
        printf(COLOR_GREY "Failed to create file.\n" COLOR_RESET);
        return 1;
    }
    
    if (strcmp(type, "c") == 0) {
        fprintf(fp, "#include <stdio.h>\n");
        fprintf(fp, "#include <stdlib.h>\n\n");
        fprintf(fp, "int main(int argc, char **argv) {\n");
        fprintf(fp, "    (void)argc; (void)argv;\n");
        fprintf(fp, "    printf(\"Hello, Void OS\\n\");\n");
        fprintf(fp, "    return 0;\n");
        fprintf(fp, "}\n");
    } else if (strcmp(type, "py") == 0) {
        fprintf(fp, "#!/usr/bin/env python3\n");
        fprintf(fp, "\"\"\"%s - Generated by Void OS\"\"\"\n\n", name);
        fprintf(fp, "def main():\n");
        fprintf(fp, "    print(\"Hello, Void OS\")\n\n");
        fprintf(fp, "if __name__ == \"__main__\":\n");
        fprintf(fp, "    main()\n");
    } else if (strcmp(type, "js") == 0) {
        fprintf(fp, "#!/usr/bin/env node\n");
        fprintf(fp, "// %s - Generated by Void OS\n\n", name);
        fprintf(fp, "function main() {\n");
        fprintf(fp, "    console.log(\"Hello, Void OS\");\n");
        fprintf(fp, "}\n\n");
        fprintf(fp, "main();\n");
    }
    
    fclose(fp);
    chmod(filename, 0755);
    
    printf(COLOR_GREEN "Created: %s\n" COLOR_RESET, filename);
    
    /* Note: Compilation requires external compiler */
    if (strcmp(type, "c") == 0) {
        printf(COLOR_GREY "Note: Compile with: gcc -o %s %s\n" COLOR_RESET, name, filename);
    }
    
    return 0;
}

int cmd_refactor(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 3) {
        printf(COLOR_GREY "Usage: refactor <old> <new> [directory]\n" COLOR_RESET);
        return 1;
    }
    
    const char *old_str = argv[1];
    const char *new_str = argv[2];
    const char *dir = (argc > 3) ? argv[3] : ".";
    
    printf(COLOR_WHITE "Refactoring: " COLOR_BLUE "'%s' -> '%s'\n" COLOR_RESET, old_str, new_str);
    printf(COLOR_WHITE "Directory: " COLOR_BLUE "%s\n" COLOR_RESET, dir);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Simple recursive search and replace */
    DIR *d = opendir(dir);
    if (d == NULL) {
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        return 1;
    }
    
    struct dirent *entry;
    int files_modified = 0;
    
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) continue;
        
        if (S_ISREG(st.st_mode)) {
            /* Read file, replace, write back */
            FILE *fp = fopen(full_path, "r");
            if (fp == NULL) continue;
            
            fseek(fp, 0, SEEK_END);
            long size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            
            char *content = malloc(size + 1);
            if (content == NULL) {
                fclose(fp);
                continue;
            }
            
            fread(content, 1, size, fp);
            content[size] = '\0';
            fclose(fp);
            
            /* Simple string replacement */
            char *pos = strstr(content, old_str);
            if (pos != NULL) {
                /* Create new content */
                size_t new_size = size + strlen(new_str) - strlen(old_str) + 1;
                char *new_content = malloc(new_size);
                if (new_content != NULL) {
                    size_t before = pos - content;
                    memcpy(new_content, content, before);
                    memcpy(new_content + before, new_str, strlen(new_str));
                    strcpy(new_content + before + strlen(new_str), pos + strlen(old_str));
                    
                    fp = fopen(full_path, "w");
                    if (fp != NULL) {
                        fwrite(new_content, 1, strlen(new_content), fp);
                        fclose(fp);
                        printf(COLOR_GREEN "Modified: %s\n" COLOR_RESET, full_path);
                        files_modified++;
                    }
                    free(new_content);
                }
            }
            
            free(content);
        }
    }
    
    closedir(d);
    
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    printf(COLOR_WHITE "Files modified: " COLOR_BLUE "%d\n" COLOR_RESET, files_modified);
    
    return 0;
}

int cmd_minify(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: minify <file> [output]\n" COLOR_RESET);
        return 1;
    }
    
    FILE *in = fopen(argv[1], "r");
    if (in == NULL) {
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        return 1;
    }
    
    const char *outfile = (argc > 2) ? argv[2] : argv[1];
    FILE *out = fopen(outfile, "w");
    if (out == NULL) {
        fclose(in);
        printf(COLOR_GREY "Failed to create output file.\n" COLOR_RESET);
        return 1;
    }
    
    int c;
    int in_comment = 0;
    int in_string = 0;
    char string_char = 0;
    int last_was_space = 0;
    
    while ((c = fgetc(in)) != EOF) {
        if (c == '"' || c == '\'') {
            if (!in_string) {
                in_string = 1;
                string_char = c;
            } else if (c == string_char) {
                in_string = 0;
            }
            fputc(c, out);
            last_was_space = 0;
        } else if (in_string) {
            fputc(c, out);
            last_was_space = 0;
        } else if (c == '/' && !in_comment) {
            int next = fgetc(in);
            if (next == '/') {
                /* Single line comment */
                while ((c = fgetc(in)) != EOF && c != '\n');
                continue;
            } else if (next == '*') {
                in_comment = 1;
                continue;
            } else {
                ungetc(next, in);
                fputc(c, out);
                last_was_space = 0;
            }
        } else if (in_comment) {
            if (c == '*') {
                int next = fgetc(in);
                if (next == '/') {
                    in_comment = 0;
                    continue;
                } else {
                    ungetc(next, in);
                }
            }
        } else if (isspace(c)) {
            if (!last_was_space && c != '\n') {
                fputc(' ', out);
                last_was_space = 1;
            }
        } else {
            fputc(c, out);
            last_was_space = 0;
        }
    }
    
    fclose(in);
    fclose(out);
    
    printf(COLOR_GREEN "Minified: %s -> %s\n" COLOR_RESET, argv[1], outfile);
    return 0;
}

int cmd_blueprint(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    const char *dir = (argc > 1) ? argv[1] : ".";
    
    printf(COLOR_WHITE "Code Analysis: %s\n" COLOR_RESET, dir);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    int total_lines = 0;
    int c_lines = 0, py_lines = 0, js_lines = 0, h_lines = 0;
    
    DIR *d = opendir(dir);
    if (d == NULL) {
        printf(COLOR_GREY "%s\n" COLOR_RESET, ERR_FILE_NOT_FOUND);
        return 1;
    }
    
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0 || !S_ISREG(st.st_mode)) continue;
        
        const char *ext = strrchr(entry->d_name, '.');
        if (ext == NULL) continue;
        
        FILE *fp = fopen(full_path, "r");
        if (fp == NULL) continue;
        
        int lines = 0;
        char line[4096];
        while (fgets(line, sizeof(line), fp) != NULL) {
            lines++;
        }
        fclose(fp);
        
        if (strcmp(ext, ".c") == 0) {
            c_lines += lines;
        } else if (strcmp(ext, ".py") == 0) {
            py_lines += lines;
        } else if (strcmp(ext, ".js") == 0) {
            js_lines += lines;
        } else if (strcmp(ext, ".h") == 0) {
            h_lines += lines;
        }
        
        total_lines += lines;
    }
    
    closedir(d);
    
    printf(COLOR_WHITE "Total Lines: " COLOR_BLUE "%d\n" COLOR_RESET, total_lines);
    if (c_lines > 0) printf(COLOR_WHITE "C:         " COLOR_BLUE "%d\n" COLOR_RESET, c_lines);
    if (h_lines > 0) printf(COLOR_WHITE "Header:    " COLOR_BLUE "%d\n" COLOR_RESET, h_lines);
    if (py_lines > 0) printf(COLOR_WHITE "Python:    " COLOR_BLUE "%d\n" COLOR_RESET, py_lines);
    if (js_lines > 0) printf(COLOR_WHITE "JavaScript:" COLOR_BLUE "%d\n" COLOR_RESET, js_lines);
    
    return 0;
}

int cmd_debug_vision(int argc, char **argv, shell_context_t *ctx) {
    (void)ctx;
    if (argc < 2) {
        printf(COLOR_GREY "Usage: debug-vision <binary>\n" COLOR_RESET);
        return 1;
    }
    
    printf(COLOR_WHITE "Debug Vision: %s\n" COLOR_RESET, argv[1]);
    printf(COLOR_GREY "─────────────────────────────────\n" COLOR_RESET);
    
    /* Native debugger implementation pending */
    /* This would require implementing ptrace() interface and ELF/DWARF parsing */
    printf(COLOR_GREY "Native debugger implementation pending.\n" COLOR_RESET);
    printf(COLOR_GREY "Full debugger requires implementing ptrace() and symbol table parsing.\n" COLOR_RESET);
    printf(COLOR_GREY "For now, use external debugger (gdb/lldb) directly.\n" COLOR_RESET);
    return 1;
}

