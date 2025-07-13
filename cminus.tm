  0:  LD  6,0(0) 	* load MP <- mem[0]
  1:  ST  0,0(0) 	* zero mem[0]
  2:  LDC  0,10(0) 	* const
  3:  ST  0,0(5) 	* store
  4:  LD  0,0(5) 	* load
  5:  LDA  1,0(0) 	* push temp to register
  6:  LD  0,0(5) 	* load
  7:  LDA  1,0(1) 	* pop temp from register to AC1
  8:  ADD  0,1,0 	* +
  9:  ST  0,1(5) 	* store
 10:  LD  0,1(5) 	* load
 11:  LDA  1,0(0) 	* push temp to register
 12:  LD  0,0(5) 	* load
 13:  LDA  1,0(1) 	* pop temp from register to AC1
 14:  ADD  0,1,0 	* +
 15:  ST  0,0(5) 	* store
 16:  HALT  0,0,0 	* finish
