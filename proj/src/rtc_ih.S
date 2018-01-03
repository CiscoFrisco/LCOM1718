#include "rtc_macros.h"

.global _g_rtc_seconds, _g_rtc_minutes, _g_rtc_hours, _g_rtc_day, _g_rtc_month, _g_rtc_year, _rtc_assembly_handler

.bss
.lcomm _g_rtc_seconds 8
.lcomm _g_rtc_minutes 8
.lcomm _g_rtc_hours	  8
.lcomm _g_rtc_day 	  8
.lcomm _g_rtc_month   8
.lcomm _g_rtc_year    8

.text

_rtc_assembly_handler:
movb 	$RTC_SECONDS, %al 		/* Read seconds and store it in its variable */
outb 	$RTC_ADDR_REG
inb 	$RTC_DATA_REG
movb 	%al, _g_rtc_seconds

movb 	$RTC_MINUTES, %al 		/* Read minutes and store it in its variable */
outb 	$RTC_ADDR_REG
inb 	$RTC_DATA_REG
movb 	%al, _g_rtc_minutes

movb	$RTC_HOURS, %al 		/* Read hours and store it in its variable */
outb 	$RTC_ADDR_REG
inb 	$RTC_DATA_REG
movb 	%al, _g_rtc_hours

movb 	$RTC_DAY, %al 			/* Read day and store it in its variable */
outb 	$RTC_ADDR_REG
inb 	$RTC_DATA_REG
movb 	%al, _g_rtc_day

movb 	$RTC_MONTH, %al 		/* Read month and store it in its variable */
outb 	$RTC_ADDR_REG
inb		$RTC_DATA_REG
movb 	%al, _g_rtc_month

movb 	$RTC_YEAR, %al 			/* Read year and store it in its variable */
outb 	$RTC_ADDR_REG
inb 	$RTC_DATA_REG
movb 	%al, _g_rtc_year

ret
