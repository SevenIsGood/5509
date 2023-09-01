#ifndef __VOLUME_H
#define __VOLUME_H

#ifndef TRUE
#define TRUE 1
#endif

#define BUF_SIZE 0x64

#define FRAMESPERBUFFER 10

#define MINGAIN 1
#define MAXGAIN 10

#define MINCONTROL 0
#define MAXCONTROL 19

#define BASELOAD 1

struct PARMS {
	int Beta;
	int EchoPower;
	int ErrorPower;
	int Ratio;
	struct PARMS *Link;
};

#endif /* __VOLUME_H */
