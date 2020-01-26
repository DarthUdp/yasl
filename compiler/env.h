#ifndef YASL_ENV_H_
#define YASL_ENV_H_

#include "data-structures/YASL_Table.h"

struct Env {
    struct Env *parent;
    struct YASL_Table vars;
    bool used_in_closure;
    size_t num_locals;   // keep track of number of locals (for when this is top level of a function).
    // TODO: keep track of which variables need to be closed over...?
};

struct Env *env_new(struct Env *const env);
void env_del(struct Env *const env);
void env_del_current_only(struct Env *const env);

size_t env_len(const struct Env *const env);
int env_contains_cur_scope(const struct Env *const env, const char *const name);
int env_contains(const struct Env *const env, const char *const name);
int64_t env_get(const struct Env *const env, const char *const name);
int64_t env_decl_var(struct Env *const env, const char *const name);
bool env_used_in_closure(const struct Env *const env, const struct Env *const fn);
void env_make_const(struct Env *const env,  const char *const name);

#endif
