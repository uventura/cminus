  0:  LD  6,0(0) 	* load MP <- mem[0]
  1:  ST  0,0(0) 	* zero mem[0]
  2:  LDC  0,10(0) 	* const
  3:  ST  0,0(5) 	* store
  4:  LDC  0,2(0) 	* const
  5:  ST  0,1(5) 	* store
  6:  LDC  0,3(0) 	* const
  7:  ST  0,2(5) 	* store
  8:  LDC  0,4(0) 	* const
  9:  ST  0,3(5) 	* store
 10:  LDC  0,5(0) 	* const
 11:  ST  0,4(5) 	* store
 12:  LDC  0,6(0) 	* const
 13:  ST  0,5(5) 	* store
 14:  LD  0,0(5) 	* load
 15:  LDA  1,0(0) 	* push temp to register
 16:  LD  0,1(5) 	* load
 17:  LDA  1,0(1) 	* pop temp from register to AC1
 18:  ADD  0,1,0 	* +
 19:  LDA  1,0(0) 	* push temp to register
 20:  LD  0,2(5) 	* load
 21:  LDA  1,0(1) 	* pop temp from register to AC1
 22:  ADD  0,1,0 	* +
 23:  LDA  1,0(0) 	* push temp to register
 24:  LD  0,3(5) 	* load
 25:  LDA  1,0(1) 	* pop temp from register to AC1
 26:  ADD  0,1,0 	* +
 27:  LDA  1,0(0) 	* push temp to register
 28:  LD  0,4(5) 	* load
 29:  LDA  1,0(1) 	* pop temp from register to AC1
 30:  ADD  0,1,0 	* +
 31:  LDA  1,0(0) 	* push temp to register
 32:  LD  0,5(5) 	* load
 33:  LDA  1,0(1) 	* pop temp from register to AC1
 34:  ADD  0,1,0 	* +
 35:  ST  0,6(5) 	* store
 36:  HALT  0,0,0 	* finish
