LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x5ff057e3b790)
PARSER: Set node name to 0x5ff057e3b800
DECLARATION: Set type 0 for identifier a
LEXER: Found ID 'b'
PARSER: Creating ID node from 'b' (pointer: 0x5ff057e3b8c0)
PARSER: Set node name to 0x5ff057e3b930
DECLARATION: Set type 0 for identifier b
LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x5ff057e3b9a0)
PARSER: Set node name to 0x5ff057e3ba10
LEXER: Found ID 'b'
PARSER: Creating ID node from 'b' (pointer: 0x5ff057e3bad0)
PARSER: Set node name to 0x5ff057e3bb40
LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x5ff057e3bb60)
PARSER: Set node name to 0x5ff057e3bbd0
LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x5ff057e3bbf0)
PARSER: Set node name to 0x5ff057e3bc60
LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x5ff057e3bd20)
PARSER: Set node name to 0x5ff057e3bd90
LEXER: Found ID 'b'
PARSER: Creating ID node from 'b' (pointer: 0x5ff057e3bdb0)
PARSER: Set node name to 0x5ff057e3be20
LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x5ff057e3be40)
PARSER: Set node name to 0x5ff057e3beb0
Program node creation
Root node set to 0x5ff057e3c010
Program syntax is correct!

===== Syntactic Tree =====
Program  (line 8)
  Block  (line 8)
    Declaration  (line 2)
      Type  [dataType=int]  (line 2)
      Identifier  [name="a"]  (line 2)
    Declaration  (line 3)
      Type  [dataType=int]  (line 3)
      Identifier  [name="b"]  (line 3)
    Assign  (line 5)
      Identifier  [name="a"]  (line 5)
      Number  [value=10]  (line 5)
    Assign  (line 6)
      Identifier  [name="b"]  (line 6)
      Plus  (line 6)
        Identifier  [name="a"]  (line 6)
        Identifier  [name="a"]  (line 6)
    Assign  (line 7)
      Identifier  [name="a"]  (line 7)
      Plus  (line 7)
        Identifier  [name="b"]  (line 7)
        Identifier  [name="a"]  (line 7)
============================

Semantic analysis:
	DEBUG: case NBlock
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: a
DEBUG: Setting type 0 for identifier a
DEBUG: Added to symbol table - a: 0
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: b
DEBUG: Setting type 0 for identifier b
DEBUG: Added to symbol table - b: 0
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: a (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: b (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: a (int), rhs: int
Program is semantically correct!
Generating code:
