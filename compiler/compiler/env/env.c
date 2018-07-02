#include "env.h"

Env_t *env_new(Env_t *parent) {
    Env_t *env  = malloc(sizeof(Env_t));
    //env->parent = malloc(sizeof(Env_t));
    //env->vars   = malloc(sizeof(Hash_t));
    env->parent = parent;
    env->vars   = ht_new();
    return env;
}

void env_del(Env_t *env) {
    if (env->parent != NULL) env_del(env->parent);
    int i;
    for (i = 0; i < env->vars->size; i++) {
        Item_t* item = env->vars->items[i];
        if (item != NULL) {
            str_del(item->key->value.sval);
            free(item->key);
            free(item->value);
            free(item);
        }
    }
    free(env->vars->items);
    free(env->vars);
    free(env->parent);
    free(env);

}

int64_t env_len(Env_t *env) {
    return env->vars->count + (env->parent == NULL ? 0 : env_len(env->parent));
}

int env_contains_cur_scope(Env_t *env, char *name, int64_t name_len) {
    String_t *string = malloc(sizeof(String_t));
    string->str = malloc(name_len);
    string->length = name_len;
    memcpy(string->str, name, string->length);
    YASL_Object key = (YASL_Object) { .value.sval = string, .type = Y_STR };

    YASL_Object *value = ht_search(env->vars, key);
    if (value == NULL) {
        return 0;
    }
    return 1;
}

int env_contains(Env_t *env, char *name, int64_t name_len) {
    String_t *string = malloc(sizeof(String_t));
    string->str = malloc(name_len);
    string->length = name_len;
    memcpy(string->str, name, string->length);
    YASL_Object key = (YASL_Object) { .value.sval = string, .type = Y_STR };

    YASL_Object *value = ht_search(env->vars, key);
    if (value == NULL && env->parent == NULL) {
        return 0;
    }
    if (value == NULL) return env_contains(env->parent, name, name_len);
    return 1;
}

int64_t env_get(Env_t *env, char *name, int64_t name_len) {
    String_t *string = malloc(sizeof(String_t));
    string->str = malloc(name_len);
    string->length = name_len;
    memcpy(string->str, name, string->length);
    YASL_Object key = (YASL_Object) { .value.sval = string, .type = Y_STR };

    YASL_Object *value = ht_search(env->vars, key);
    if (value == NULL && env->parent == NULL) {
        printf("error in env_get with key: ");
        print(key);
        exit(EXIT_FAILURE);
    }
    if (value == NULL) return env_get(env->parent, name, name_len);
    return value->value.ival;
}

void env_decl_var(Env_t *env, char *name, int64_t name_len) {
    String_t *string = malloc(sizeof(String_t));
    string->str = malloc(name_len);
    string->length = name_len;
    memcpy(string->str, name, string->length);
    YASL_Object key = (YASL_Object) { .value.sval = string, .type = Y_STR };
    YASL_Object value = (YASL_Object) { .value.ival = env_len(env), .type = Y_INT64 };
    ht_insert(env->vars, key, value);
}