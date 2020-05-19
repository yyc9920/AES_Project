@REM #########################################################################
@REM #
@REM #  Copyright (c) 2005 - 2012 by Vivante Corp.  All rights reserved.
@REM #
@REM #  The material in this file is confidential and contains trade secrets
@REM #  of Vivante Corporation. This is proprietary information owned by
@REM #  Vivante Corporation. No part of this work may be disclosed,
@REM #  reproduced, copied, transmitted, or used in any way for any purpose,
@REM #  without the express written permission of Vivante Corporation.
@REM #
@REM #########################################################################


@echo off
set ARGV=320_240_A8R8G8B8_VIRTUAL 320_240_A8R8G8B8_DEFAULT 320_240_A1R5G5B5_VIRTUAL 320_240_A4R4G4B4_VIRTUAL 320_240_R5G6B5_SYSTEM 640_480_A8R8G8B8_DEFAULT
set ARGV=%ARGV% 640_480_R8G8B8A8_VIRTUAL 640_480_R5G5B5A1_VIRTUAL 640_480_B8G8R8A8_VIRTUAL 640_480_B5G6R5_VIRTUAL 640_480_A4B4G4R4_VIRTUAL 600_400_B4G4R4A4_DEFAULT

for %%i in (%ARGV%) do (
	echo %%i
	mkdir %%i\bmp %%i\err %%i\log %%i\rlt
	call run.bat %%i.ini
	mv *.bmp %%i\bmp
	mv *.log %%i\log
	mv *.rlt %%i\rlt
	mv *.err %%i\err
)

:end
