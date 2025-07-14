LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x63ccd4f5c790)
PARSER: Set node name to 0x63ccd4f5c800
DECLARATION: Set type 0 for identifier a
LEXER: Found ID 'b'
PARSER: Creating ID node from 'b' (pointer: 0x63ccd4f5c8c0)
PARSER: Set node name to 0x63ccd4f5c930
DECLARATION: Set type 0 for identifier b
LEXER: Found ID 'c'
PARSER: Creating ID node from 'c' (pointer: 0x63ccd4f5c9f0)
PARSER: Set node name to 0x63ccd4f5ca60
DECLARATION: Set type 0 for identifier c
LEXER: Found ID 'd'
PARSER: Creating ID node from 'd' (pointer: 0x63ccd4f5cb20)
PARSER: Set node name to 0x63ccd4f5cb90
DECLARATION: Set type 0 for identifier d
LEXER: Found ID 'e'
PARSER: Creating ID node from 'e' (pointer: 0x63ccd4f5cc50)
PARSER: Set node name to 0x63ccd4f5ccc0
DECLARATION: Set type 0 for identifier e
LEXER: Found ID 'f'
PARSER: Creating ID node from 'f' (pointer: 0x63ccd4f5cd80)
PARSER: Set node name to 0x63ccd4f5cdf0
DECLARATION: Set type 0 for identifier f
LEXER: Found ID 'result'
PARSER: Creating ID node from 'result' (pointer: 0x63ccd4f5ceb0)
PARSER: Set node name to 0x63ccd4f5cf20
DECLARATION: Set type 0 for identifier result
LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x63ccd4f5cf90)
PARSER: Set node name to 0x63ccd4f5d000
LEXER: Found ID 'b'
PARSER: Creating ID node from 'b' (pointer: 0x63ccd4f5d0c0)
PARSER: Set node name to 0x63ccd4f5d130
LEXER: Found ID 'c'
PARSER: Creating ID node from 'c' (pointer: 0x63ccd4f5d1f0)
PARSER: Set node name to 0x63ccd4f5d260
LEXER: Found ID 'd'
PARSER: Creating ID node from 'd' (pointer: 0x63ccd4f5d320)
PARSER: Set node name to 0x63ccd4f5d390
LEXER: Found ID 'e'
PARSER: Creating ID node from 'e' (pointer: 0x63ccd4f5d450)
PARSER: Set node name to 0x63ccd4f5d4c0
LEXER: Found ID 'f'
PARSER: Creating ID node from 'f' (pointer: 0x63ccd4f5d580)
PARSER: Set node name to 0x63ccd4f5d5f0
LEXER: Found ID 'result'
PARSER: Creating ID node from 'result' (pointer: 0x63ccd4f5d6b0)
PARSER: Set node name to 0x63ccd4f5d720
LEXER: Found ID 'a'
PARSER: Creating ID node from 'a' (pointer: 0x63ccd4f5d740)
PARSER: Set node name to 0x63ccd4f5d7b0
LEXER: Found ID 'b'
PARSER: Creating ID node from 'b' (pointer: 0x63ccd4f5d7d0)
PARSER: Set node name to 0x63ccd4f5d840
LEXER: Found ID 'c'
PARSER: Creating ID node from 'c' (pointer: 0x63ccd4f5d8b0)
PARSER: Set node name to 0x63ccd4f5d920
LEXER: Found ID 'd'
PARSER: Creating ID node from 'd' (pointer: 0x63ccd4f5d990)
PARSER: Set node name to 0x63ccd4f5da00
LEXER: Found ID 'e'
PARSER: Creating ID node from 'e' (pointer: 0x63ccd4f5da70)
PARSER: Set node name to 0x63ccd4f5dae0
LEXER: Found ID 'f'
PARSER: Creating ID node from 'f' (pointer: 0x63ccd4f5db50)
PARSER: Set node name to 0x63ccd4f5dbc0
Program node creation
Root node set to 0x63ccd4f5dd20
Program syntax is correct!

===== Syntactic Tree =====
Program  (line 21)
  Block  (line 21)
    Declaration  (line 3)
      Type  [dataType=int]  (line 3)
      Identifier  [name="a"]  (line 3)
    Declaration  (line 4)
      Type  [dataType=int]  (line 4)
      Identifier  [name="b"]  (line 4)
    Declaration  (line 5)
      Type  [dataType=int]  (line 5)
      Identifier  [name="c"]  (line 5)
    Declaration  (line 6)
      Type  [dataType=int]  (line 6)
      Identifier  [name="d"]  (line 6)
    Declaration  (line 7)
      Type  [dataType=int]  (line 7)
      Identifier  [name="e"]  (line 7)
    Declaration  (line 8)
      Type  [dataType=int]  (line 8)
      Identifier  [name="f"]  (line 8)
    Declaration  (line 9)
      Type  [dataType=int]  (line 9)
      Identifier  [name="result"]  (line 9)
    Assign  (line 11)
      Identifier  [name="a"]  (line 11)
      Number  [value=10]  (line 11)
    Assign  (line 12)
      Identifier  [name="b"]  (line 12)
      Number  [value=2]  (line 12)
    Assign  (line 13)
      Identifier  [name="c"]  (line 13)
      Number  [value=3]  (line 13)
    Assign  (line 14)
      Identifier  [name="d"]  (line 14)
      Number  [value=4]  (line 14)
    Assign  (line 15)
      Identifier  [name="e"]  (line 15)
      Number  [value=5]  (line 15)
    Assign  (line 16)
      Identifier  [name="f"]  (line 16)
      Number  [value=6]  (line 16)
    Assign  (line 18)
      Identifier  [name="result"]  (line 18)
      Plus  (line 18)
        Plus  (line 18)
          Plus  (line 18)
            Plus  (line 18)
              Plus  (line 18)
                Identifier  [name="a"]  (line 18)
                Identifier  [name="b"]  (line 18)
              Identifier  [name="c"]  (line 18)
            Identifier  [name="d"]  (line 18)
          Identifier  [name="e"]  (line 18)
        Identifier  [name="f"]  (line 18)
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
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: c
DEBUG: Setting type 0 for identifier c
DEBUG: Added to symbol table - c: 0
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: d
DEBUG: Setting type 0 for identifier d
DEBUG: Added to symbol table - d: 0
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: e
DEBUG: Setting type 0 for identifier e
DEBUG: Added to symbol table - e: 0
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: f
DEBUG: Setting type 0 for identifier f
DEBUG: Added to symbol table - f: 0
	DEBUG: case NDeclaration
	DEBUG: Checking declaration, name: result
DEBUG: Setting type 0 for identifier result
DEBUG: Added to symbol table - result: 0
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: a (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: b (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: c (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: d (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: e (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: f (int), rhs: int
	DEBUG: case NAssignStmt / NAssign
DEBUG: Assignment type check - lhs: result (int), rhs: int
Program is semantically correct!
Generating code:
