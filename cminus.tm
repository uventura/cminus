  0:  LD  6,0(0) 	* load MP <- mem[0]
  1:  ST  0,0(0) 	* zero mem[0]
  2:  LDC  0,10(0) 	* const
  3:  MOV  2,0,0 	* x
  4:  LDC  0,20(0) 	* const
  5:  MOV  3,0,0 	* y
  6:  MOV  0,2,0 	* x
  7:  ST  0,-1(6) 	* push
  8:  MOV  0,3,0 	* y
  9:  LD  1,-1(6) 	* pop
 10:  ADD  0,1,0 	* +
 11:  MOV  2,0,0 	* x
 12:  HALT  0,0,0 	* finish
