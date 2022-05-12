#include <map>
#include <vector>

#include "AST.h"
#include "token.h"
#include "types.h"

class typechecker {
public:
    typedef enum {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT
    } associativity_t;

    typedef struct {
        type_t type;
        std::string name;
    } function_parameter_t;

    typedef struct {
        std::string name;
        type_t return_type;
        std::vector<function_parameter_t> parameters;
    } function_t;

public:
    std::vector<token> tokens;
    std::map<std::string, int> precedenceMap;
    std::map<std::string, associativity_t> associativityMap;

    std::map<std::string, type_t> global_variables;
    std::map<std::string, type_t> local_variables;

    std::vector<function_t> functions;

    int currentTokenIndex;

    std::vector<std::string> type_decl_list;
    std::vector<std::string> type_error_list;

    bool global = true;
    bool is_in_function = false;

    std::string function_name = "";
    type_t function_return_type = UNDEFINED;

    bool found_return_statement = false;

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
    }

public:
    std::unique_ptr<ASTNode> Program();

    std::unique_ptr<ASTNode> Decl();

    std::unique_ptr<ASTNode> DeclList();

    std::unique_ptr<ASTNode> VarDecl();

    std::vector<std::string> VarDeclList(type_t type);

    std::string VarDeclId(type_t type);

    std::unique_ptr<ASTNode> FuncDecl();

    void FuncDeclParamList(std::vector<function_parameter_t> *function_);

    std::unique_ptr<ASTNode> Block();

    std::unique_ptr<ASTNode> Stmt();

    std::unique_ptr<ASTNode> StmtList();

    std::unique_ptr<ASTNode> Expr();

    type_t Type();

    std::string Id();

    std::unique_ptr<ASTNode> ComputeExpression(int precedence);

    std::unique_ptr<ASTNode> ComputeTerm();
//
//    void FuncCallParamList(std::string name, std::vector<type_t> *params);

private:
    static std::string type_to_string(type_t type) {
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

    std::string type_to_string2(type_t type, const std::string &s) {
        switch (type) {
            case INTEGER:
                return "int";
            case CHAR:
                return "char";
            case STRING:
                return string_format("char %s[]", s.c_str());
            case VOID:
                return "void";
            case BOOL:
                return "bool";
            case DOUBLE:
                return "double";
            case FLOAT:
                return "float";
            case INTEGER_ARRAY:
                return string_format("int %s[]", s.c_str());
            case CHAR_ARRAY:
                return string_format("char %s[]", s.c_str());
            case STRING_ARRAY:
                return string_format("char[] %s[]", s.c_str());
            case BOOL_ARRAY:
                return string_format("bool %s[]", s.c_str());
            case DOUBLE_ARRAY:
                return string_format("double %s[]", s.c_str());
            case FLOAT_ARRAY:
                return string_format("float %s[]", s.c_str());
            default:
                return "ERROR";
        }
    }

    static bool is_array_type(type_t type) {
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
            case UNDEFINED:
                return false;
            case VOID:
                return false;
            case INTEGER:
                return false;
            case CHAR:
                return false;
            case DOUBLE:
                return false;
            case FLOAT:
                return false;
            case BOOL:
                return false;
        }
    }

    static type_t array_base_type(type_t type) {
        switch (type) {
            case INTEGER_ARRAY:
                return INTEGER;
            case CHAR_ARRAY:
                return CHAR;
            case STRING_ARRAY:
                return STRING;
            case BOOL_ARRAY:
                return BOOL;
            case DOUBLE_ARRAY:
                return DOUBLE;
            case FLOAT_ARRAY:
                return FLOAT;
            default:
                return UNDEFINED;
        }

        return UNDEFINED;
    }

    template<typename ... Args>
    std::string string_format(const std::string &format, Args ... args) {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
        auto size = static_cast<size_t>( size_s );
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }

    static void error(const std::string &expectedToken, const std::string &tokenValue, const std::string &fileName,
                      int lineNumber) {

        if (tokenValue == "EOF") {
            std::fprintf(stderr, "Parser error in file %s line %i near end of file\n\t Expected '%s'", fileName.c_str(),
                         lineNumber,
                         expectedToken.c_str());
        } else {
            std::fprintf(stderr, "Parser error in file %s line %i near text %s\n\t Expected '%s'", fileName.c_str(),
                         lineNumber,
                         tokenValue.c_str(), expectedToken.c_str());
        }

        exit(EXIT_FAILURE);
    }

    static void error_no_quotes(const std::string &expectedToken, const std::string &tokenValue,
                                const std::string &fileName,
                                int lineNumber) {
        if (tokenValue == "EOF") {
            std::fprintf(stderr, "Parser error in file %s line %i near end of file\n\t Expected %s", fileName.c_str(),
                         lineNumber,
                         expectedToken.c_str());
        } else {
            std::fprintf(stderr, "Parser error in file %s line %i near text %s\n\t Expected %s", fileName.c_str(),
                         lineNumber,
                         tokenValue.c_str(), expectedToken.c_str());
        }

        exit(EXIT_FAILURE);
    }

    token peak_next_token() {
        return tokens[currentTokenIndex + 1];
    }

    token consume() {
        token token;

        if (currentTokenIndex != -1) {
            token = tokens[currentTokenIndex];
        }

        currentTokenIndex += 1;

        return token;
    }

    token rollback() {
        token token;

        if (currentTokenIndex != -1) {
            token = tokens[currentTokenIndex];
        }

        currentTokenIndex -= 1;

        return token;
    }

    bool match(token_type type) {
        if (peak_next_token().type == type) {
            return true;
        }

        return false;
    }
};