#ifndef _PROJ_RTC_MACROS_H_
#define _PROJ_RTC_MACROS_H_

/** @defgroup rtc_macros rtc_macros
 * @{
 *
 * Constants for programming the RTC.
 */

#define BIT(n) 		 (0x01<<(n))	/**< @brief Macro to create a number with bit n set */

#define RTC_DATA_REG 0x71 			/**< @brief Port of RTC's data register */

#define RTC_ADDR_REG 0x70 			/**< @brief Port of RTC's address register */

#define RTC_SECONDS  0				/**< @brief Offset of RTC's seconds register */

#define RTC_MINUTES  2				/**< @brief Offset of RTC's minutes register */

#define RTC_HOURS    4				/**< @brief Offset of RTC's hours register */

#define RTC_DAY      7				/**< @brief Offset of RTC's day register */

#define RTC_MONTH    8				/**< @brief Offset of RTC's month register */

#define RTC_YEAR     9				/**< @brief Offset of RTC's year register */

#define RTC_REG_A    10				/**< @brief Offset of RTC's Register A */

#define RTC_REG_B    11				/**< @brief Offset of RTC's Register B */

#define RTC_REG_C    12				/**< @brief Offset of RTC's Register C */

#define RTC_UIP      0x80   		/**< @brief Bit 7 of RTC's Register A */

#define disINT       0x8F			/**< @brief Bitmask to disable RTC interrupts */

#define enINT        0x70			/**< @brief Bitmask to enable RTC interrupts */

#define enPINT       0x40			/**< @brief Bitmask to enable RTC periodic interrupts */

#define set24        0x02			/**< @brief Bitmask to set 24 hours range */

#define setBCD       0xFB 			/**< @brief Bitmask to set data format to BCD */

#define RTC_IRQ      8				/**< @brief RTC's IRQ line */

/**@}  end of rtc_macros */

#endif /* _PROJ_RTC_MACROS_H_ */
