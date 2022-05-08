#include <map>
#include <vector>

#include "token.h"

class typechecker {
public:
    typedef enum {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT
    } associativity_t;

    typedef enum {
        UNDEFINED = 0,
        VOID = 1,
        INTEGER = 2,
        CHAR = 3,
        DOUBLE = 4,
        STRING = 5,
        FLOAT = 6,
        BOOL = 7,
        INTEGER_ARRAY = 8,
        CHAR_ARRAY = 9,
        DOUBLE_ARRAY = 10,
        STRING_ARRAY = 11,
        FLOAT_ARRAY = 12,
        BOOL_ARRAY = 13,
    } type_t;

    typedef struct {
        type_t type;
        std::string name;
    } function_parameter_t;

    typedef struct {
        std::string name;
        type_t return_type;
        std::vector<function_parameter_t> parameters;
    } function_t;

    typedef struct {
        type_t type;
        std::string name;
        std::string value;
    } variable_t;

public:
    std::vector<token> tokens;
    std::map<std::string, int> precedenceMap;
    std::map<std::string, associativity_t> associativityMap;

    std::map<std::string, type_t> global_variables;
    std::map<std::string, type_t> local_variables;

    std::vector<function_t> functions;

    int currentTokenIndex;

    bool global = true;

public:
    typechecker(std::vector<token> tokens) {
        this->tokens = tokens;
        currentTokenIndex = -1;

        // A → A α | β
        // A → β A'
        // A' → α A' | ϵ

        // E –> E + T | T
        // T –> T * F | F
        // F –> ( E ) | id

        // E –> T E’
        // E’ –> + T E’ | e
        // T –> F T’
        // T’ –> * F T’ | e
        // F –> ( E ) | id

        // Expr         := Expr '+' MultiExpr | MultiExpr
        // MultiExpr    := MultiExpr '*' Literal

        // Expr         := MultiExpr Expr2 | MultiExpr
        // Expr2        :=  '+' MultiExpr Expr2 | e

        // MultiExpr    := Literal MultiExpr2 | Literal
        // MultiExpr2   := '*' Literal MultiExpr2 | e


        // MultiExpr        := Expr '*' Expr | IntegerLiteral
        // IntegerLiteral   := 0-9*

        this->precedenceMap["("] = 12;
        this->precedenceMap[")"] = 12;
        this->precedenceMap["["] = 12;
        this->precedenceMap["]"] = 12;
        this->precedenceMap["."] = 12;

        this->precedenceMap["!"] = 11;
        this->precedenceMap["~"] = 11;
        this->precedenceMap["-"] = 11; // - (Unary)
        this->precedenceMap["--"] = 11;

        // Type
        this->precedenceMap["int"] = 11;
        this->precedenceMap["double"] = 11;
        this->precedenceMap["char"] = 11;
        this->precedenceMap["string"] = 11;

        this->precedenceMap["*"] = 10;
        this->precedenceMap["/"] = 10;
        this->precedenceMap["%"] = 10;

        this->precedenceMap["+"] = 9;
        this->precedenceMap["-"] = 9;

        this->precedenceMap["<"] = 8;
        this->precedenceMap["<="] = 8;
        this->precedenceMap[">"] = 8;
        this->precedenceMap[">="] = 8;

        this->precedenceMap["=="] = 7;
        this->precedenceMap["!="] = 7;

        this->precedenceMap["&"] = 6;

        this->precedenceMap["|"] = 5;

        this->precedenceMap["&&"] = 4;

        this->precedenceMap["||"] = 3;

        this->precedenceMap["?"] = 2;
        this->precedenceMap[":"] = 2;

        this->precedenceMap["+="] = 1;
        this->precedenceMap["-="] = 1;
        this->precedenceMap["*="] = 1;
        this->precedenceMap["/="] = 1;
        this->precedenceMap["%="] = 1;

        this->precedenceMap[","] = 0;


        this->associativityMap["("] = LEFT_TO_RIGHT;
        this->associativityMap[")"] = LEFT_TO_RIGHT;
        this->associativityMap["["] = LEFT_TO_RIGHT;
        this->associativityMap["]"] = LEFT_TO_RIGHT;
        this->associativityMap["."] = LEFT_TO_RIGHT;

        this->associativityMap["!"] = RIGHT_TO_LEFT;
        this->associativityMap["~"] = RIGHT_TO_LEFT;
        this->associativityMap["-"] = RIGHT_TO_LEFT; // - (Unary)
        this->associativityMap["--"] = RIGHT_TO_LEFT;

        this->associativityMap["int"] = LEFT_TO_RIGHT;
        this->associativityMap["char"] = LEFT_TO_RIGHT;
        this->associativityMap["double"] = LEFT_TO_RIGHT;
        this->associativityMap["string"] = LEFT_TO_RIGHT;

        this->associativityMap["*"] = LEFT_TO_RIGHT;
        this->associativityMap["/"] = LEFT_TO_RIGHT;
        this->associativityMap["%"] = LEFT_TO_RIGHT;

        this->associativityMap["+"] = LEFT_TO_RIGHT;
        this->associativityMap["-"] = LEFT_TO_RIGHT;

        this->associativityMap["<"] = LEFT_TO_RIGHT;
        this->associativityMap["<="] = LEFT_TO_RIGHT;
        this->associativityMap[">"] = LEFT_TO_RIGHT;
        this->associativityMap[">="] = LEFT_TO_RIGHT;

        this->associativityMap["=="] = LEFT_TO_RIGHT;
        this->associativityMap["!="] = LEFT_TO_RIGHT;

        this->associativityMap["&"] = LEFT_TO_RIGHT;

        this->associativityMap["|"] = LEFT_TO_RIGHT;

        this->associativityMap["&&"] = LEFT_TO_RIGHT;

        this->associativityMap["||"] = LEFT_TO_RIGHT;

        // ?:
        this->associativityMap["?"] = RIGHT_TO_LEFT;
        this->associativityMap[":"] = RIGHT_TO_LEFT;

        this->associativityMap["+="] = RIGHT_TO_LEFT;
        this->associativityMap["-="] = RIGHT_TO_LEFT;
        this->associativityMap["*="] = RIGHT_TO_LEFT;
        this->associativityMap["/="] = RIGHT_TO_LEFT;
        this->associativityMap["%="] = RIGHT_TO_LEFT;

        this->associativityMap[","] = LEFT_TO_RIGHT;

//        // Installing printf functions.
//        function_t printf_function_str;
//
//        printf_function_str.name = "printf";
//        printf_function_str.return_type = VOID;
//
//        function_parameter_t printf_str_parameter;
//
//        printf_str_parameter.type = STRING;
//        printf_str_parameter.name = "str";
//
//        printf_function_str.parameters.push_back(printf_str_parameter);
//
//
//        function_t printf_function_int_int;
//
//        printf_function_int_int.name = "printf";
//        printf_function_int_int.return_type = VOID;
//
//        function_parameter_t printf_int_parameter_1;
//        function_parameter_t printf_int_parameter_2;
//
//        printf_int_parameter_1.type = INTEGER;
//        printf_int_parameter_1.name = "i1";
//
//        printf_int_parameter_2.type = INTEGER;
//        printf_int_parameter_2.name = "i2";
//
//        printf_function_int_int.parameters.push_back(printf_int_parameter_1);
//        printf_function_int_int.parameters.push_back(printf_int_parameter_2);
//
//
//        // Install putchar function.
//        function_t putc_function;
//
//        putc_function.name = "putc";
//        putc_function.return_type = VOID;
//
//        function_parameter_t putc_int_parameter;
//
//        putc_int_parameter.type = CHAR;
//        putc_int_parameter.name = "char";
//
//        putc_function.parameters.push_back(putc_int_parameter);
//
//
//        function_t exit_function;
//
//        exit_function.name = "exit";
//        exit_function.return_type = VOID;
//
//        function_parameter_t exit_int_parameter;
//
//        exit_int_parameter.name = "status";
//        exit_int_parameter.type = INTEGER;
//
//        exit_function.parameters.push_back(exit_int_parameter);
//
//        this->functions.push_back(printf_function_str);
//        this->functions.push_back(printf_function_int_int);
//        this->functions.push_back(putc_function);
//        this->functions.push_back(exit_function);
    }

private:
    static void error(const std::string& expectedToken, const std::string& tokenValue, const std::string& fileName, int lineNumber);

    static void error_no_quotes(const std::string& expectedToken, const std::string& tokenValue, const std::string& fileName, int lineNumber);

    static void unexpected_token_error(const std::string& tokenValue, const std::string& fileName, int lineNumber);

    /**
     * Gets the next token without consuming the current token.
     * @return the next token.
     */
    token peak_next_token();

    /**
     * Gets the next n token.
     * @param n the offset.
     * @return the nth token.
     */
    token peak(int n);

    /**
     * Checks if the next token_type is the same as the excepted.
     * @param type the expected token_type.
     * @return if the type matches.
     */
    bool match(token_type type);

    /**
     * Gets the current token and advance the current token index.
     * @return the current token.
     */
    token consume();

public:
    void Program();

    void Decl();

    void DeclList();

    void VarDecl();

    void VarDeclList(type_t type);

    void VarDeclId(type_t type);

    void FuncDecl();

    void FuncDeclParamList(std::vector<function_parameter_t> *function_);

    void Block();

    void Stmt();

    void StmtList();

    type_t Expr();

    type_t Type();

    std::string Id();

    type_t ComputeExpression(int precedence);

    type_t ComputeTerm();

    token rollback();

    void FuncCallParamList(std::string name, std::vector<type_t> *params);

private:
    std::string type_to_string(type_t type) {
        switch (type) {
            case INTEGER:
                return "int";
            case CHAR:
                return "char";
            case STRING:
                return "char[]";
            case VOID:
                return "void";
            case BOOL:
                return "bool";
            case DOUBLE:
                return "double";
            case FLOAT:
                return "float";
            case INTEGER_ARRAY:
                return "int[]";
            case CHAR_ARRAY:
                return "char[]";
            case STRING_ARRAY:
                return "string[]";
            case BOOL_ARRAY:
                return "bool[]";
            case DOUBLE_ARRAY:
                return "double[]";
            case FLOAT_ARRAY:
                return "float[]";
            default:
                return "ERROR";
        }
    }

    bool is_array_type(type_t type) {
        switch (type) {
            case STRING:
                return true;
            case INTEGER_ARRAY:
                return true;
            case CHAR_ARRAY:
                return true;
            case STRING_ARRAY:
                return true;
            case BOOL_ARRAY:
                return true;
            case DOUBLE_ARRAY:
                return true;
            case FLOAT_ARRAY:
                return true;
            default:
                return false;
        }
    }
};