### Lexer 

TWO versions of lexer: A) With regex, and B) Without regex, or any manner of third-party libraries, using only raw string comparisons and state machines
    

---

Create a simple lexical analyzer that reads a sample program and outputs the corresponding tokens it interpreted. Use basic string matching and regular expressions to identify tokens. For instance, the following code sample

```
fn int my_fn(int x, float y) {
    string my_str = "hmm";
    bool my_bool = x == 40;
    return x;
}
```

Should get tokenized as:

`[T_FUNCTION, T_INT, T_IDENTIFIER("my_fn"), T_PARENL, T_INT, T_IDENTIFIER("x"), T_COMMA, T_FLOAT, T_IDENTIFIER("y"), T_PARENR, T_BRACEL, T_STRING, T_IDENTIFIER("my_str"), T_ASSIGNOP, T_QUOTES, T_STRINGLIT("HMM"), T_QUOTES, T_SEMICOLON, T_BOOL, T_IDENTIFIER("MY_BOOL"), T_ASSIGNOP, T_IDENTIFIER("x"), T_EQUALSOP, T_INTLIT(40), T_SEMICOLON, T_RETURN, T_IDENTIFIER("x"), T_SEMICOLON, T_BRACER]`

Any other common constructs not shown in the sample, such as conditional statements (if, else), loops, comparison and logical operators (<=, !=, >=, &&, ||, etc.), bitwise operators, brackets ([]), parentheses (()), braces ({}), additional data types (float and bool), and comments should also be tokenized correctly.

The lexer should handle escaped characters (e.g, \", \t, \n, etc.) within string literals as well. Errors should be thrown upon the detection of an invalid identifier e.g variable names can not start with a number.

**Bonus:** Unicode character support e.g emojis or non-latin characters for identifier names, string literals, etc.
