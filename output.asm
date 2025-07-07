LEXER: Found ID 'x'
PARSER: Creating ID node from 'x' (pointer: 0x5f734b106790)
PARSER: Set node name to 0x5f734b106800
DECLARATION: Set type 0 for identifier x
LEXER: Found ID 'y'
PARSER: Creating ID node from 'y' (pointer: 0x5f734b1068c0)
PARSER: Set node name to 0x5f734b106930
DECLARATION: Set type 0 for identifier y
LEXER: Found ID 'x'
PARSER: Creating ID node from 'x' (pointer: 0x5f734b1069a0)
PARSER: Set node name to 0x5f734b106a10
LEXER: Found ID 'y'
PARSER: Creating ID node from 'y' (pointer: 0x5f734b106ad0)
PARSER: Set node name to 0x5f734b106b40
LEXER: Found ID 'x'
PARSER: Creating ID node from 'x' (pointer: 0x5f734b106c00)
PARSER: Set node name to 0x5f734b106c70
LEXER: Found ID 'x'
PARSER: Creating ID node from 'x' (pointer: 0x5f734b106c90)
PARSER: Set node name to 0x5f734b106d00
LEXER: Found ID 'y'
PARSER: Creating ID node from 'y' (pointer: 0x5f734b106d20)
PARSER: Set node name to 0x5f734b106d90
Program node creation
Root node set to 0x5f734b106ef0
Program syntax is correct!

===== Syntactic Tree =====
Program  (line 11)
  Block  (line 11)
    Declaration  (line 3)
      Type  [dataType=int]  (line 3)
      Identifier  [name="x"]  (line 3)
    Declaration  (line 4)
      Type  [dataType=int]  (line 4)
      Identifier  [name="y"]  (line 4)
    Assign  (line 6)
      Identifier  [name="x"]  (line 6)
      Number  [value=10]  (line 6)
    Assign  (line 7)
      Identifier  [name="y"]  (line 7)
      Number  [value=20]  (line 7)
    Assign  (line 9)
      Identifier  [name="x"]  (line 9)
      Plus  (line 9)
        Identifier  [name="x"]  (line 9)
        Identifier  [name="y"]  (line 9)
============================

Semantic analysis:
	DEBUG: case NBlock
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: x
DEBUG: Setting type 0 for identifier x
DEBUG: Added to symbol table - x: 0
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: y
DEBUG: Setting type 0 for identifier y
DEBUG: Added to symbol table - y: 0
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: x (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: y (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: x (int), rhs: int
Program is semantically correct!
