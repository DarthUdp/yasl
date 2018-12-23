#pragma once

#include <inttypes.h>
#include "../ast/ast.h"
#include "../parser/parser.h"
#include "../bytebuffer/bytebuffer.h"
#include "../../opcode.h"
#include "env/env.h"
#include "../../debug.h"

struct Compiler {
    Parser *parser;
    Env_t *globals;
    Env_t *params;
    size_t offset;
    struct Table *strings;
    ByteBuffer *buffer;
    ByteBuffer *header;
    ByteBuffer *code;
    size_t *checkpoints;
    size_t checkpoints_count;
    size_t checkpoints_size;
    int status;
};

struct Compiler *compiler_new(Parser *const parser);
void compiler_del(struct Compiler *compiler);
unsigned char *compile(struct Compiler *const compiler);