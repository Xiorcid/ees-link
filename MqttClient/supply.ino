void xy_update_test(){
  tMemory Mem;
  Mem.Nr = 2;
  Mem.VSet = 1200;
  Mem.ISet =  500;
  Mem.sLVP = 1000;
  Mem.sOVP = 1300;
  Mem.sOCP =  620;
  Mem.sOPP = 1040;
  Mem.sOHPh= 0;
  Mem.sOHPm= 0;
  Mem.sOAH = 0;
  Mem.sOWH = 0;
  Mem.sOTP = 110;
  Mem.sINI = 0;
  xy.SetMemory(Mem);
  // xy.PrintMemory(Mem);
}