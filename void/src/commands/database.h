#ifndef DATABASE_H
#define DATABASE_H

#include "../../include/void.h"

/* Category 8: Database Operations (50 commands) */

/* SQL Databases */
int cmd_mysql(int argc, char **argv, shell_context_t *ctx);
int cmd_mysqladmin(int argc, char **argv, shell_context_t *ctx);
int cmd_mysqldump(int argc, char **argv, shell_context_t *ctx);
int cmd_psql(int argc, char **argv, shell_context_t *ctx);
int cmd_pg_dump(int argc, char **argv, shell_context_t *ctx);
int cmd_pg_restore(int argc, char **argv, shell_context_t *ctx);
int cmd_sqlite3(int argc, char **argv, shell_context_t *ctx);
int cmd_sqlcmd(int argc, char **argv, shell_context_t *ctx);

/* NoSQL Databases */
int cmd_mongo(int argc, char **argv, shell_context_t *ctx);
int cmd_mongosh(int argc, char **argv, shell_context_t *ctx);
int cmd_redis_cli(int argc, char **argv, shell_context_t *ctx);
int cmd_cassandra_cli(int argc, char **argv, shell_context_t *ctx);
int cmd_cqlsh(int argc, char **argv, shell_context_t *ctx);
int cmd_hbase_shell(int argc, char **argv, shell_context_t *ctx);
int cmd_neo4j(int argc, char **argv, shell_context_t *ctx);

/* Database Tools */
int cmd_mongodump(int argc, char **argv, shell_context_t *ctx);
int cmd_redis_dump(int argc, char **argv, shell_context_t *ctx);

#endif /* DATABASE_H */

