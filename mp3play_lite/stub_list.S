	.set noreorder

#include "pspstub.s"

	STUB_START "LoadExecForUser",0x40090000,0x00010005
	STUB_FUNC  0xD1FB50DC,LoadExecForUser_D1FB50DC
	STUB_END

	#3.5xNID(Use NID_Resolver)
	STUB_START "sceDmacplus_driver",0x00010011,0x00010005
	STUB_FUNC  0x29B50A82,sceDmacplusLcdcSetBaseAddr
	STUB_END

