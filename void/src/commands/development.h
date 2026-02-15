#ifndef DEVELOPMENT_H
#define DEVELOPMENT_H

#include "../../include/void.h"

/* Category 7: Development Tools (150 commands) */

/* Version Control */
int cmd_svn(int argc, char **argv, shell_context_t *ctx);
int cmd_hg(int argc, char **argv, shell_context_t *ctx);
int cmd_bzr(int argc, char **argv, shell_context_t *ctx);
int cmd_cvs(int argc, char **argv, shell_context_t *ctx);
int cmd_fossil_scm(int argc, char **argv, shell_context_t *ctx);

/* Build Systems */
int cmd_make_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_cmake(int argc, char **argv, shell_context_t *ctx);
int cmd_autoconf(int argc, char **argv, shell_context_t *ctx);
int cmd_automake(int argc, char **argv, shell_context_t *ctx);
int cmd_configure(int argc, char **argv, shell_context_t *ctx);
int cmd_ninja(int argc, char **argv, shell_context_t *ctx);
int cmd_bazel(int argc, char **argv, shell_context_t *ctx);
int cmd_buck(int argc, char **argv, shell_context_t *ctx);
int cmd_gradle(int argc, char **argv, shell_context_t *ctx);
int cmd_maven(int argc, char **argv, shell_context_t *ctx);
int cmd_ant(int argc, char **argv, shell_context_t *ctx);
int cmd_scons(int argc, char **argv, shell_context_t *ctx);
int cmd_waf(int argc, char **argv, shell_context_t *ctx);

/* Compilers & Interpreters */
int cmd_gcc(int argc, char **argv, shell_context_t *ctx);
int cmd_gpp(int argc, char **argv, shell_context_t *ctx);
int cmd_clang_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_clangpp(int argc, char **argv, shell_context_t *ctx);
int cmd_rustc(int argc, char **argv, shell_context_t *ctx);
int cmd_go_compiler(int argc, char **argv, shell_context_t *ctx);
int cmd_javac(int argc, char **argv, shell_context_t *ctx);
int cmd_python_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_python3_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_ruby_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_node_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_perl_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_php_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_lua_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_tclsh(int argc, char **argv, shell_context_t *ctx);

/* Code Analysis */
int cmd_cppcheck(int argc, char **argv, shell_context_t *ctx);
int cmd_clang_tidy(int argc, char **argv, shell_context_t *ctx);
int cmd_pylint(int argc, char **argv, shell_context_t *ctx);
int cmd_flake8(int argc, char **argv, shell_context_t *ctx);
int cmd_mypy(int argc, char **argv, shell_context_t *ctx);
int cmd_eslint(int argc, char **argv, shell_context_t *ctx);
int cmd_jshint(int argc, char **argv, shell_context_t *ctx);
int cmd_rubocop(int argc, char **argv, shell_context_t *ctx);
int cmd_golint(int argc, char **argv, shell_context_t *ctx);
int cmd_gometalinter(int argc, char **argv, shell_context_t *ctx);
int cmd_staticcheck(int argc, char **argv, shell_context_t *ctx);
int cmd_shellcheck(int argc, char **argv, shell_context_t *ctx);

/* Code Formatting */
int cmd_clang_format(int argc, char **argv, shell_context_t *ctx);
int cmd_indent(int argc, char **argv, shell_context_t *ctx);
int cmd_astyle(int argc, char **argv, shell_context_t *ctx);
int cmd_uncrustify(int argc, char **argv, shell_context_t *ctx);
int cmd_black(int argc, char **argv, shell_context_t *ctx);
int cmd_autopep8(int argc, char **argv, shell_context_t *ctx);
int cmd_yapf(int argc, char **argv, shell_context_t *ctx);
int cmd_prettier(int argc, char **argv, shell_context_t *ctx);
int cmd_js_beautify(int argc, char **argv, shell_context_t *ctx);
int cmd_gofmt(int argc, char **argv, shell_context_t *ctx);
int cmd_rustfmt(int argc, char **argv, shell_context_t *ctx);

/* Testing */
int cmd_pytest(int argc, char **argv, shell_context_t *ctx);
int cmd_unittest_ext(int argc, char **argv, shell_context_t *ctx);
int cmd_nose(int argc, char **argv, shell_context_t *ctx);
int cmd_mocha(int argc, char **argv, shell_context_t *ctx);
int cmd_jest(int argc, char **argv, shell_context_t *ctx);
int cmd_rspec(int argc, char **argv, shell_context_t *ctx);
int cmd_go_test(int argc, char **argv, shell_context_t *ctx);
int cmd_cargo_test(int argc, char **argv, shell_context_t *ctx);
int cmd_ctest(int argc, char **argv, shell_context_t *ctx);

#endif /* DEVELOPMENT_H */

