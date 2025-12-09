#ifndef TYPECHK_H
#define TYPECHK_H

#include "ast.h"
#include "scope.h"


typedef enum {
    TC_NOERR = 0, // No error. Example: int x = 5; // correct declaration
    TC_ErroneousVarDecl, // Variable declaration error. Example: int x = "hello"; // type mismatch
    TC_FnCallParamCount, // Incorrect number of function call parameters. Example: foo(1, 2); // but foo expects 1 param
    TC_FnCallParamType, // Incorrect function call parameter type. Example: foo("str"); // but foo expects int
    TC_ErroneousReturnType, // Return type does not match function declaration. Example: int foo() { return "str"; }
    TC_ExpressionTypeMismatch, // General type mismatch in expressions. Example: 1 + "abc";
    TC_ExpectedBooleanExpression, // Non-boolean used where boolean expected. Example: if (5) { ... } // expects bool
    TC_ErroneousBreak, //! Misplaced break statement. Example: break; // outside of loop/switch
    TC_NonBooleanCondStmt, // Non-boolean used in condition statement. Example: while (42) { ... } // expects bool
    TC_EmptyExpression, // Expression is empty or missing. Example: ; // empty statement where expression expected
    TC_AttemptedBoolOpOnNonBools, // Boolean operator used on non-bools. Example: 5 && 10; // expects bools
    TC_AttemptedBitOpOnNonNumeric, //! Bitwise operator on non-numeric types. Example: "abc" & 3;
    TC_AttemptedShiftOnNonInt, // Shift operator on non-integers. Example: 3.14 << 2;
    TC_AttemptedAddOpOnNonNumeric, // Addition operator on non-numeric types. Example: true + "abc";
    TC_AttemptedExponentiationOfNonNumeric, // Exponentiation on non-numeric types. Example: "abc" ** 2;
    TC_ReturnStmtNotFound //! No return statement found in non-void function. Example: int foo() { } // missing return
} TypeChkError;

typedef struct TypeChkReport {
    int error_count;
} TypeChkReport;

/* Run the type checker. The global scope produced by analyze_scopes must be passed. */
TypeChkReport type_check(Node *root, Scope *global);

#endif
