#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0) -> Attribute);

struct InputBuffer_t {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
};

typedef struct InputBuffer_t InputBuffer;

enum MetaCommandResult_t {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};

typedef enum MetaCommandResult_t MetaCommandResult;

enum PrepareResult_t {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
};

typedef enum PrepareResult_t PrepareResult;

enum StatementType_t {
    STATEMENT_INSERT,
    STATEMENT_SELECT
};

typedef enum StatementType_t StatementType;

struct Statement_t {
    StatementType type;
};

typedef struct Statement_t Statement;

const uint32_t COLOUMN_USERNAME_SIZE = 32;
const uint32_t COLUMN_EMAIL_SIZE = 255;

struct Row_t {
    uint32_t id;
    char username[COLOUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
};

typedef struct Row_t Row;

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 100;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

struct Table_t {
    void* pages[TABLE_MAX_PAGES];
    uint32_t num_rows;
};

typedef struct Table_t Table;

Table* newTable() {
    Table* table = malloc(sizeof(Table));
    table->num_rows = 0;

    return table;
}

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt() {
    printf("db > ");
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
    switch(statement->type) {
        case (STATEMENT_INSERT):
            printf("This is where we would do an insert. \n");
            break;
        case (STATEMENT_SELECT):
            printf("This is where we would do a select. \n");
            break;
    }
}

void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read -1] = 0;
}

void serialize_row(Row* source, void* destination) {
    mem_cpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    mem_cpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    mem_cpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
    mem_cpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    mem_cpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    mem_cpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

int main(int argc, char* argv[])
{
    InputBuffer* input_buffer = new_input_buffer();
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch(do_meta_command(input_buffer)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        execute_statement(&statement);
        printf("Executed.\n");
    }
}
