void delay();
Uint16 BlockWr[64]={0};
void BootClk(Uint16 ClkPhase)
 {
   if(ClkPhase==0)
    {MCBSP_FSET(PCR0,CLKXP,0);}
   else
    {MCBSP_FSET(PCR0,CLKXP,1);}
   }
void BootTx(Uint16 TxPhase)
 {
   if(TxPhase==0)
    {MCBSP_FSET(PCR0,DXSTAT,0);}
   else
    {MCBSP_FSET(PCR0,DXSTAT,1);}
   }
Uint16 BootRx(void)
 {
   Uint16 RxTmp;
   RxTmp=MCBSP_FGET(PCR0,DRSTAT);
   return(RxTmp);
   }
void BootSyn(Uint16 IO4Phase)
 {
   if(IO4Phase==0)
    {GPIO_FSET(IODATA,IO4D,0);}
   else
    {GPIO_FSET(IODATA,IO4D,1);}
   }
Uint16 sRd1Byte(void)
 {
       Uint16 i,RdDatTemp;
       Uint16 Rdbit;
       RdDatTemp=0x00;
       for(i=0;i<8;i++)
        {
              BootClk(0);
              BootClk(1);
              Rdbit=BootRx();
              RdDatTemp<<=1; 
              if(Rdbit==1)
                {RdDatTemp|=0x01;}
              else
                {RdDatTemp&=0xfe;} 
          }
        return(RdDatTemp);   
   }
void sWr1Byte(Uint16 WrDatP)
   {
     Uint16 WrDatPTmp,i;
	 WrDatPTmp=WrDatP;
	  for(i=0;i<8;i++)
	   {
	    WrDatPTmp&=0x80;
		 if(WrDatPTmp==0x80)
		  { 
		    BootTx(1);
		   }
         else
		  {
		    BootTx(0);
		    }
		  BootClk(0);
		  BootClk(1);
		  WrDatP<<=1;
		  WrDatPTmp=WrDatP;
	    } 
     }
void EepromWrEn(void)
   {
     BootSyn(1);
	 BootSyn(0);
     sWr1Byte(0x06);
     BootSyn(1);
    }
void EepromWrDi(void)
   {
     BootSyn(1);
	 BootSyn(0);
     sWr1Byte(0x04);
     BootSyn(1);
    }
Uint16 EepromRdReg(void)
   {
    Uint16 RdTemp;
     BootSyn(1);
	 BootSyn(0);
     sWr1Byte(0x05);
     RdTemp=sRd1Byte();
     BootSyn(1);	 
	 return(RdTemp);
    }
void EepromWrReg(Uint16 RegP)
   {
     BootSyn(1);
	 BootSyn(0);
     sWr1Byte(0x01);
     sWr1Byte(RegP);
     BootSyn(1);
    }
void ChkWIP(void)
   {
    Uint16 BusyTemp;
    BusyTemp=EepromRdReg();
   while(BusyTemp==0x01)
    {
	  BusyTemp=(BusyTemp&0x01);
	  }
    }   
Uint16 EepromRd(Uint16 RdAddr)
   {
     Uint16  Tmp;
     Uint16 WrDatTmp;
	 Uint16 RdDatTmp;
	 BootSyn(1);
	 BootSyn(0);
     sWr1Byte(0x03);
 	 Tmp=RdAddr;
	 WrDatTmp=(Tmp>>8);
	 sWr1Byte(WrDatTmp);
     Tmp=RdAddr;
	 WrDatTmp=(Tmp&0x00ff);
     sWr1Byte(WrDatTmp);
     RdDatTmp=sRd1Byte();
     BootSyn(1);
	 return(RdDatTmp);
    }       
void EepromWr(Uint16 WrAddr)
   {
     Uint16  Tmp;
     Uint16 WrDatTmp;
     Uint16  i;
     EepromWrEn();
	 BootSyn(1);
	 BootSyn(0);
     sWr1Byte(0x02);
 	 Tmp=WrAddr;
	 WrDatTmp=(Tmp>>8);
	 sWr1Byte(WrDatTmp);
     Tmp=WrAddr;
	 WrDatTmp=(Tmp&0x00ff);
     sWr1Byte(WrDatTmp);    
      for(i=0;i<64;i++)
        {
          WrDatTmp=BlockWr[i]; 
          sWr1Byte(WrDatTmp);
          }
	 BootSyn(1);
	 ChkWIP();
    } 
