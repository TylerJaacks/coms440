//
// Created by Tyler Jaacks on 3/6/22.
//

// ReSharper disable CppInconsistentNaming
// ReSharper disable CppClangTidyConcurrencyMtUnsafe

#include "parser.h"

token parser::peak_previous_token() {
    return tokens[currentTokenIndex - 1];
}

token parser::peak_next_token() {
    return tokens[currentTokenIndex + 1];
}

token parser::consume_token()  {
    token token = tokens[currentTokenIndex];

    currentTokenIndex += 1;

    return token;
}

// S := Program
void parser::S() {
    Program();
}

// Program := DeclList Decl | Decl
void parser::Program() {
    DeclList();
    Decl();
}

// DeclList := DeclList Decl | Decl
void parser::DeclList() {
    DeclList();
    Decl();
}

// Decl := VarDecl | FuncDecl
void parser::Decl() {
    VarDecl();
    // FuncDecl();
}

// VarDecl := Type VarDeclList ;
void parser::VarDecl() {
    Type();
    VarDeclList();

    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume_token();
    }
    else {
        fprintf(stderr, "ERROR: Expected a ';' but got a %s at the end of a variable declaration.", peak_next_token().value.c_str());
    }
}

void parser::ScopedVarDecl() {
    if (peak_next_token().type == TOKEN_KEYWORD_STATIC) {
        consume_token();
    }
    Type();
    VarDeclList();

    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume_token();
    }
    else {
        fprintf(stderr, "ERROR: Expected a ';' but got a %s at the end of a variable declaration.", peak_next_token().value.c_str());
    }
}

// VarDeclList := VarDeclList ',' VarDeclInit | VarDeclInit
void parser::VarDeclList() {
    VarDeclList();

    if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
        consume_token();
        VarDeclInit();
    }
    else {
        VarDeclInit();
    }
}

// VarDeclInit := VarDeclId
void parser::VarDeclInit() {
    VarDeclId();
}

// VarDeclId := Id | Id '[' NumberConstant ']'
void parser::VarDeclId() {
    VarDeclId();

    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
        consume_token();

        NumberConstant();

        if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
            consume_token();
        }
        else {
            fprintf(stderr, "ERROR: Expected a ']' but got a %s at the end of a variable initialization of array.", peak_next_token().value.c_str());
        }
    }
    else {
        fprintf(stderr, "ERROR: Expected a '[' but got a %s at the end of a variable initialization of array.", peak_next_token().value.c_str());
    }
}

// Type := 'int' | 'char' | 'double' | 'void'
void parser::Type() {
    if (peak_next_token().type == TOKEN_TYPE_INTEGER) {
        consume_token();
    }
    else if (peak_next_token().type == TOKEN_TYPE_DOUBLE) {
        consume_token();
    }
    else if (peak_next_token().type == TOKEN_TYPE_CHAR) {
        consume_token();
    }
    else if (peak_next_token().type == TOKEN_TYPE_VOID) {
        consume_token();
    }
    else {
        fprintf(stderr, "ERROR: Expected a type but got %s instead.", peak_next_token().value.c_str());
    }
}

// FuncDecl := Type Id '(' Params ')' Stmt
void parser::FuncDecl() {
    Type();
    Id();

    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
        consume_token();

        Params();

        if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
            Stmt();
        }
        else {
            fprintf(stderr, "ERROR: Expected a ')' at the beginning of a function declaration but got %s instead.", peak_next_token().value.c_str());
        }
    }
    else {
        fprintf(stderr, "ERROR: Expected a '(' at the beginning of a function declaration but got %s instead.", peak_next_token().value.c_str());
    }
}

// Params := ParamList | e
void parser::Params() {
    ParamList();
}

// ParamList := ParamTypeList
void parser::ParamList() {
    ParamTypeList();
}

// ParamTypeList := Type ParamIdList
void parser::ParamTypeList() {
    Type();
    ParamIdList();
}

// ParamIdList := ParamIdList ',' ParamId | ParamId
void parser::ParamIdList() {
    ParamIdList();

    if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
        consume_token();
        ParamId();
    }

    ParamId();
}

// ParamId := ID | ID '[' ']'
void parser::ParamId() {
    Id();

    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET)
    {
        consume_token();

        if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET)
        {
            consume_token();
        }
		else {
			fprintf(stderr, "ERROR: Expected a ']' but got a %s at the end of a variable.", peak_next_token().value.c_str());
		}
    }
	else {
		fprintf(stderr, "ERROR: Expected a '[' but got a %s at the end of a variable.", peak_next_token().value.c_str());
	}
}

// Stmt := ExprStmt | CompoundStmt | IfStmt | WhileStmt | DoWhileStmt | ForStmt | ReturnStmt | BreakStmt
void parser::Stmt()
{
	ExprStmt();
	CompoundStmt();
	IfStmt();
	WhileStmt();
	DoWhileStmt();
	ForStmt();
	ReturnStmt();
	BreakStmt();
}

// ExprStmt := Expr ';' | ';'
void parser::ExprStmt()
{
    Expr();

    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON)
    {
        consume_token();
    }
}

// CompoundStmt := '{' LocalDecls StmtList '}'
void parser::CompoundStmt()
{
	if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACE)
	{
        consume_token();

		if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACE)
		{
            consume_token();

            LocalDecls();
            StmtList();
		}
        else
        {
            fprintf(stderr, "");
            exit(EXIT_FAILURE);
        }
	}
    else
    {
        fprintf(stderr, "");
        exit(EXIT_FAILURE);
    }
}

// LocalDecls := LocalDecls ScopedVarDecl | empty
void parser::LocalDecls()
{
    LocalDecls();
    ScopedVarDecl();
}

// StmtList := StmtList Stmt | empty
void parser::StmtList()
{
    StmtList();
    Stmt();
}

// StmtList := StmtList Stmt | empty
void parser::ReturnStmt()
{
    if (peak_next_token().type == TOKEN_KEYWORD_RETURN)
    {
        consume_token();

        Expr();

        if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON)
        {
            fprintf(stderr, "");
            exit(EXIT_FAILURE);
        }
    }
}

// BreakStmt := 'break' ';'
void parser::BreakStmt()
{
    if (peak_next_token().type == TOKEN_KEYWORD_BREAK)
    {
        consume_token();

		if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON)
		{
			fprintf(stderr, "");
			exit(EXIT_FAILURE);
		}
    }
}

// TODO: Finish
// Expr := 
//    Mutable '='   Expr | 
//    Mutable '+='  Expr | 
//    Mutable '-='  Expr | 
//    Mutable '*='  Expr | 
//    Mutable '/='  Expr |
//    Mutable '%='  Expr |
//    Mutable '<<=' Expr |
//    Mutable '>>=' Expr |
//    Mutable '&='  Expr |
//    Mutable '|='  Expr |
//    Mutable '^='  Expr |
//    Mutable '++'  |
//    Mutable '--'  |
//    SimpleExpr
void parser::Expr()
{
    Mutable();
}

// IfStmt := 
//     'if' '(' SimpleExp ')' CompoundStmt |
//     'if' '(' SimpleExp ')' CompoundStmt 'else' CompoundStmt | 
//     'if' '(' SimpleExp ')' CompoundStmt 'else' IfStmt
void parser::IfStmt()
{
    if (peak_next_token().type == TOKEN_PREPROCESSOR_IF)
    {
        consume_token();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) 
        {
            consume_token();

            SimpleExpr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN)
            {
                consume_token();

                CompoundStmt();

                if (peak_next_token().type == TOKEN_KEYWORD_ELSE)
                {
                    consume_token();

                    if (peak_next_token().type == TOKEN_KEYWORD_IF)
                    {
                        consume_token();

                        IfStmt();
                    }

                    CompoundStmt();
                }
            }
            else
            {
				fprintf(stderr, "");
				exit(EXIT_FAILURE);
            }
        }
        else
        {
			fprintf(stderr, "");
			exit(EXIT_FAILURE);
        }
    }
}

// WhileStmt := 'while' '(' SimpleExp ')' CompoundStmt
void parser::WhileStmt()
{
	if (peak_next_token().type == TOKEN_KEYWORD_WHILE)
	{
        consume_token();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN)
        {
            consume_token();

            SimpleExpr();

			if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN)
			{
                consume_token();

                CompoundStmt();
			}
        }
	}
}

// DoWhileStmt := 'do' CompoundStmt 'while' '(' SimpleExp ')' ';'
void parser::DoWhileStmt()
{
    if (peak_next_token().type == TOKEN_KEYWORD_DO)
    {
        consume_token();

        CompoundStmt();

        if (peak_next_token().type == TOKEN_KEYWORD_WHILE)
        {
            consume_token();

            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN)
            {
                consume_token();

                SimpleExpr();

                if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN)
                {
                    consume_token();

                    if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON)
                    {
                        fprintf(stderr, "");
                    }
                }
				else
				{
					fprintf(stderr, "");
					exit(EXIT_FAILURE);
				}
            }
            else
            {
                fprintf(stderr, "");
                exit(EXIT_FAILURE);
            }
        }
    }
}

// ForStmt := 'for' '(' VarDecl SimpleExp ';' Expr ')' CompoundStmt
void parser::ForStmt()
{
    if (peak_next_token().type == TOKEN_KEYWORD_FOR)
    {
        consume_token();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
            consume_token();

            VarDecl();
            SimpleExpr();

            if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON)
            {
				fprintf(stderr, "");
				exit(EXIT_FAILURE);
            }

            Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN)
            {
                CompoundStmt();
            }
            else
            {
				fprintf(stderr, "");
				exit(EXIT_FAILURE);
            }
        }
        else
        {
			fprintf(stderr, "");
			exit(EXIT_FAILURE);
        }
    }
}

// SimpleExpr := SimpleExpr '||' AndExpr | AndExpr
void parser::SimpleExpr()
{
    SimpleExpr();

    if (peak_next_token().type == TOKEN_SYMBOL_PIPE)
    {
        consume_token();

		if (peak_next_token().type == TOKEN_SYMBOL_PIPE)
		{
			consume_token();

            AndExpr();
		}
        else
        {
			fprintf(stderr, "");
			exit(EXIT_FAILURE);
        }
    }
    else
    {
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
    }
}

// AndExpr := SimpleExpr '&&' UnaryRelExpr | UnaryRelExpr
void parser::AndExpr()
{
	SimpleExpr();

	if (peak_next_token().type == TOKEN_SYMBOL_AMPERSAND)
	{
		consume_token();

		if (peak_next_token().type == TOKEN_SYMBOL_AMPERSAND)
		{
			consume_token();

            UnaryRelExpr();
		}
		else
		{
			fprintf(stderr, "");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
	}
}

// UnaryRelExpr := '!' UnaryRelExpr | RelExpr
void parser::UnaryRelExpr()
{
	if (peak_next_token().type == TOKEN_SYMBOL_EXCLAMATION_MARK)
	{
        consume_token();

        UnaryRelExpr();
        RelExpr();
	}
}

// RelExpr := SumExpr RelOp SumExpr | SumExpr
void parser::RelExpr()
{
    SumExpr();
    RelOp();
    SumExpr();
}

// TODO: Finish
// RelOp := <= | < | > | >= | == | !=
void parser::RelOp()
{
	
}

// SumExpr := SumExpr SumOp MultExpr | MultExpr
void parser::SumExpr() {
	SumExpr();
	SumOp();
	MultExpr();
}

// TODO: Finish
// SumOp := '+' | '-'
void parser::SumOp()
{
    if (peak_next_token().type != TOKEN_SYMBOL_PLUS_SIGN ||
	    peak_next_token().type != TOKEN_SYMBOL_PLUS_SIGN)
    {
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
    }

	consume_token();
}

// MultExpr := MultExpr MultOp UnaryRelExpr | UnaryRelExpr
void parser::MultExpr() {
	MultExpr();
	MultOp();
	UnaryRelExpr();
}

// MultOp := '*' | '/' | '%'
void parser::MultOp()
{
	if (peak_next_token().type != TOKEN_SYMBOL_ASTERISK ||
		peak_next_token().type != TOKEN_SYMBOL_FORWARD_SLASH ||
        peak_next_token().type != TOKEN_SYMBOL_PERCENT_SIGN)
	{
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
	}

    consume_token();
}

// Factor := Immutable | Mutable
void parser::Factor()
{
    Immutable();
    Mutable();
}

// Mutable := ID | ID '[' Expr ']' | Call | Constant
void parser::Mutable()
{
    Id();

    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) 
    {
        consume_token();

        Expr();

        if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_BRACKET)
        {
            fprintf(stderr, "");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "");
        exit(EXIT_FAILURE);
    }

    Call();
    Constant();
}

// Immutable := '(' Expr ')' | Call | Constant
void parser::Immutable()
{
	
}

// Call := ID '(' Args ')' ';'
void parser::Call()
{
    Id();

	if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN)
	{
		consume_token();

		Args();

		if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN)
		{
			fprintf(stderr, "");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
	}
}

// Args := ArgList | empty 
void parser::Args()
{
    ArgsList();
}

// ArgList := ArgList ',' Expr | Expr
void parser::ArgsList()
{
    ArgsList();

    if (peak_next_token().type == TOKEN_SYMBOL_COMMA)
    {
        consume_token();

        Expr();
    }

    Expr();
}

// Constant := NumberConstant | DoubleConstant | CharConst | StringConstant
void parser::Constant()
{
    NumberConstant();
    DoubleConstant();
    CharConstant();
    StringConstant();
}

void parser::NumberConstant()
{
	if (peak_next_token().type != TOKEN_LITERAL_NUMBER)
	{
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
	}

	consume_token();
}

void parser::DoubleConstant()
{
	if (peak_next_token().type != TOKEN_TYPE_DOUBLE)
	{
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
	}

	consume_token();
}

void parser::CharConstant()
{
	if (peak_next_token().type != TOKEN_LITERAL_CHAR)
	{
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
	}

	consume_token();
}

void parser::StringConstant()
{
    if (peak_next_token().type != TOKEN_LITERAL_STRING)
    {
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
    }

    consume_token();
}

void parser::Id()
{
	if (peak_next_token().type != TOKEN_IDENTIFIER)
	{
		fprintf(stderr, "");
		exit(EXIT_FAILURE);
	}

    consume_token();
}