  0:  LDC  5,0(0) 	* Initialize GP to 0 (base for static data)
  1:  LDC  6,1023(0) 	* Initialize MP to top of memory (stack base, grows down)
  2:  LDC  0,0(0) 	* Clear AC (accumulator)
  3:  ST  0,0(0) 	* Clear mem[0] (or initialize global 0 constant)
  4:  LDC  0,10(0) 	* load constant
  5:  ST  0,0(5) 	* store variable
  6:  LDC  0,2(0) 	* load constant
  7:  ST  0,1(5) 	* store variable
  8:  LDC  0,5(0) 	* load constant
  9:  ST  0,2(5) 	* store variable
 10:  LDC  0,2(0) 	* load constant
 11:  ST  0,3(5) 	* store variable
 12:  LDC  0,5(0) 	* load constant
 13:  ST  0,4(5) 	* store variable
 14:  LDC  0,6(0) 	* load constant
 15:  ST  0,5(5) 	* store variable
 16:  LD  0,0(5) 	* load identifier
 17:  LDA  2,0(0) 	* push temp to register
 18:  LD  0,1(5) 	* load identifier
 19:  LDA  3,0(0) 	* push temp to register
 20:  LD  0,2(5) 	* load identifier
 21:  LDA  1,0(3) 	* pop temp from register to AC1
 22:  MUL  0,1,0 	* multiply
 23:  LDA  3,0(0) 	* push temp to register
 24:  LD  0,3(5) 	* load identifier
 25:  LDA  1,0(3) 	* pop temp from register to AC1
 26:  DIV  0,1,0 	* divide
 27:  LDA  1,0(2) 	* pop temp from register to AC1
 28:  ADD  0,1,0 	* add
 29:  LDA  2,0(0) 	* push temp to register
 30:  LD  0,4(5) 	* load identifier
 31:  LDA  1,0(2) 	* pop temp from register to AC1
 32:  SUB  0,1,0 	* subtract
 33:  LDA  2,0(0) 	* push temp to register
 34:  LD  0,5(5) 	* load identifier
 35:  LDA  1,0(2) 	* pop temp from register to AC1
 36:  ADD  0,1,0 	* add
 37:  ST  0,6(5) 	* store variable
 38:  HALT  0,0,0 	* finish
