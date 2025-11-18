#ifndef TYPECHK_H
#define TYPECHK_H

#include "ast.h"
#include "scope.h"

typedef enum {
    TC_NOERR = 0,
    TC_ErroneousVarDecl,
    TC_FnCallParamCount,
    TC_FnCallParamType,
    TC_ErroneousReturnType,
    TC_ExpressionTypeMismatch,
    TC_ExpectedBooleanExpression,
    TC_ErroneousBreak,
    TC_NonBooleanCondStmt,
    TC_EmptyExpression,
    TC_AttemptedBoolOpOnNonBools,
    TC_AttemptedBitOpOnNonNumeric,
    TC_AttemptedShiftOnNonInt,
    TC_AttemptedAddOpOnNonNumeric,
    TC_AttemptedExponentiationOfNonNumeric,
    TC_ReturnStmtNotFound
} TypeChkError;

typedef struct TypeChkReport {
    int error_count;
} TypeChkReport;

/* Run the type checker. The global scope produced by analyze_scopes must be passed. */
TypeChkReport type_check(Node *root, Scope *global);

#endif
