#include "hscdtd008a.h"
#include "hscdtd008a_reg.h"
#include "platform.h"

#include <Arduino.h>

// Define some macros to make reading and writing registers easier.
#define read_register(R, P) t_read_register(HSCDTD_ADDR, R, 1, (uint8_t *) P)
#define write_register(R, P) t_write_register(HSCDTD_ADDR, R, 1, (uint8_t *) P)


/**
 * @brief Initialize the device.
 *
 * @return 0 on success.
 */
int8_t initialize(void)
{
	int8_t i;
	int8_t status;

	// Open transport.
	t_open();

	// Wait a bit for the I2C bus to open. 
	t_sleep_ms(100);

	// Reset the chip to make sure register have expected values.
	// Some chips behave weird when starting up. So we have to try a bunch
	// of times before we can actually properly communicate with the chip.
	for (i = 0; i < 10; i++) {
		status = soft_reset();
		if (status == 0)
			break;
		t_sleep_ms(5);
	}

	// Check if reset went OK.
	if (status != 0)
		return status;

	// Wait bit before getting started.
	t_sleep_ms(50);

	// Check Who I Am
	status = who_i_am_check();
	if (status != 0)
		return status;

	// Set output resolution to 15 bits.
	status = set_resolution(HSCDTD_RESOLUTION_15_BIT);
	if (status != 0)
		return status;

	// Set the device to active.
	status = set_mode(HSCDTD_MODE_ACTIVE);
	if (status != 0)
		return status;
	
	// Do a selftest
	status = self_test();
	if (status != 0)
		return status;

	return 0;
}


/* --------------------------------------------------
 * CTRL1 Settings
 */


/**
 * @brief Set the functional mode of the device.
 *
 * Device can be in the following modes:
 *  - Stand-by mode (Default)
 *  - Active mode.
 *
 * Stand-by mode is the default mode of the device.
 *
 * This is also referred to as 'Power Mode' in the datasheet.
 *
 * See the state machine on page 5 of the datasheet
 * for more information.
 *
 * @param mode Desired device mode.
 * @return 0 on success.
 */
int8_t set_mode(HSCDTD_CTRL1_PC_t mode)
{
	int8_t status;
	HSCDTD_CTRL1_t reg;

	status = read_register(HSCDTD_REG_CTRL1, &reg);
	if (status != 0)
		return status;
	
	reg.PC = mode;

	status = write_register(HSCDTD_REG_CTRL1, &reg);
	if (status != 0)
		return status;

	return 0;
}


/**
 * @brief Set the output data rate.
 *
 * Data rate can be configured as to the
 * following speeds:
 *  - 0.5Hz
 *  - 10H (Default)
 *  - 20Hz
 *  - 100Hz.
 *
 * Output Data Rate (ODR) only applies if the device
 * is in the 'Normal' state. This setting has no effect
 * in the 'Force' state.
 *
 * @return 0 on success.
 */
int8_t set_output_data_rate(HSCDTD_CTRL1_ODR_t odr)
{
	int8_t status;
	HSCDTD_CTRL1_t reg;

	status = read_register(HSCDTD_REG_CTRL1, &reg);
	if (status != 0)
		return status;
	
	reg.ODR = odr;

	status = write_register(HSCDTD_REG_CTRL1, &reg);
	if (status != 0)
		return status;

	return 0;
}


/**
 * @brief Set the state of the device.
 *
 * There are two valid states for the device:
 *  - Normal State
 *  - Force State (Default).
 *
 * Configure the device to use the NORMAL or FORCE state.
 *
 * See the state machine on page 5 of the datasheet
 * for more information.
 *
 * @param state New state for the device.
 * @return 0 on success.
 */
int8_t set_state(HSCDTD_CTRL1_FS_t state)
{
	int8_t status;
	HSCDTD_CTRL1_t reg;

	status = read_register(HSCDTD_REG_CTRL1, &reg);
	if (status != 0)
		return status;
	
	reg.FS = state;

	status = write_register(HSCDTD_REG_CTRL1, &reg);
	if (status != 0)
		return status;

	return 0;
}


/* --------------------------------------------------
 * CTRL2 Settings
 */


/**
 * @brief Set the fifo data storage method.
 *
 * Set the method for storing data in FIFO. There are 2
 * valid modes:
 *  - Direct (Default)
 *  - Comparision.
 *
 * This functionality is only available if FIFO is enabled.
 *
 * If storage method is set to 'Comparision' refer to page 11
 * of the datasheet for more information.
 *
 * @param fco Storage method
 * @return 0 on success.
 */
int8_t set_fifo_data_storage_method(HSCDTD_CTRL2_FCO_t fco)
{
	int8_t status;
	HSCDTD_CTRL2_t reg;

	status = read_register(HSCDTD_REG_CTRL2, &reg);
	if (status != 0)
		return status;
	
	reg.FCO = fco;

	status = write_register(HSCDTD_REG_CTRL2, &reg);
	if (status !=0)
		return status;
	
	return 0;
}


/**
 * @brief Set the fifo comparision method.
 *
 * If the FIFO storage method is configured to 'Comparision' the,
 * method can be configured using this register.
 *
 * There are two valid methods:
 *  - 'Or' comparision (Default)
 *  - 'And' comparision.
 *
 * Refer to page 11 of the datasheet for more information on corret values.
 *
 * This functionality is only available if FIFO is enabled.
 *
 * @param aor Comparision method.
 * @return 0 on success.
 */
int8_t set_fifo_comparision_method(HSCDTD_CTRL2_AOR_t aor)
{
	int8_t status;
	HSCDTD_CTRL2_t reg;
	
	status = read_register(HSCDTD_REG_CTRL2, &reg);
	if (status != 0)
		return status;
	
	reg.AOR = aor;

	status = write_register(HSCDTD_REG_CTRL2, &reg);
	if (status !=0)
		return status;
	
	return 0;
}


/**
 * @brief Set the fifo enable status.
 *
 * Set the enabled status of the FIFO functionality.
 *
 * There are only two modes:
 *  - Disabled (Default)
 *  - Enabled.
 *
 * Refer to page 10 of the datasheet for more information.
 *
 * @param ff Fifo enable status.
 * @return 0 on success.
 */
int8_t set_fifo_enable(HSCDTD_CTRL2_FF_t ff)
{
	int8_t status;
	HSCDTD_CTRL2_t reg;
	
	status = read_register(HSCDTD_REG_CTRL2, &reg);
	if (status != 0)
		return status;
	
	reg.FF = ff;

	status = write_register(HSCDTD_REG_CTRL2, &reg);
	if (status !=0)
		return status;
	
	return 0;
}


/**
 * @brief Set the data ready pin enable status.
 *
 * Configure if the Data Ready (DRDY) status should be output
 * on the DRDY Pin.
 *
 * There are two modes:
 *  - Disabled (Default)
 *  - Enabled.
 *
 * For more infomation refer to page 8 of the datasheet.
 *
 * @param den Data Ready Pin enable status.
 * @return 0 on success.
 */
int8_t set_data_ready_pin_enable(HSCDTD_CTRL2_DEN_t den)
{
	int8_t status;
	HSCDTD_CTRL2_t reg;
	
	status = read_register(HSCDTD_REG_CTRL2, &reg);
	if (status != 0)
		return status;
	
	reg.DEN = den;

	status = write_register(HSCDTD_REG_CTRL2, &reg);
	if (status !=0)
		return status;
	
	return 0;
}


/**
 * @brief Set the data ready pin polarity.
 *
 * Configure the polarity of the DRDY Pin.
 *
 * The polarity can be configured as:
 *  - Active LOW
 *  - Active HIGH (Default).
 *
 * This functionality is only available if the
 * output control of the DRDY pin is enabled.
 *
 * Refer to page 8 of the datasheet for more information.
 *
 * @param drp DRP setting.
 * @return 0 on success.
 */
int8_t set_data_ready_pin_polarity(HSCDTD_CTRL2_DRP_t drp)
{
	int8_t status;
	HSCDTD_CTRL2_t reg;
	
	status = read_register(HSCDTD_REG_CTRL2, &reg);
	if (status != 0)
		return status;
	
	reg.DRP = drp;

	status = write_register(HSCDTD_REG_CTRL2, &reg);
	if (status !=0)
		return status;
	
	return 0;
}


/* --------------------------------------------------
 * CTRL4 Settings
 */

/**
 * @brief Set the resolution of the output.
 *
 * There are two resolution configurations for the device:
 *  - 14 bit (-8192 to +8191) (Default)
 *  - 15 bit (-16384 to +16383)
 *
 * At 15 bit output the resolution is 0.150uT/LSB.
 * The resolution for 14 bit is not specified.
 *
 * @param resolution Resolution to configure.
 * @return 0 on success.
 */
int8_t set_resolution(HSCDTD_CTRL4_RS_t resolution)
{
	int8_t status;
	HSCDTD_CTRL4_t reg;

	status = read_register(HSCDTD_REG_CTRL4, &reg);
	if (status != 0)
		return status;
	
	reg.RS = resolution;

	status = write_register(HSCDTD_REG_CTRL4, &reg);
	if (status != 0)
		return status;

	return 0;
}

/* --------------------------------------------------
 * Device functionality
 */


/**
 * @brief Check if the contents of the Who I Am register
 * is correct.
 * 
 * @return int8_t 
 */
int8_t who_i_am_check(void)
{
	int8_t status;
	uint8_t reg;
	
	// The datasheet refers to the WIA regster
	status = read_register(HSCDTD_REG_WIA, &reg);
	if (status != 0)
		return status;

	// Value should be 0x49 according to the datashset.
	if (reg != 0x49)
		return -1;
	return 0;
}

/**
 * @brief Start ADC offset calibration.
 *
 * Device must be in the force state.
 *
 * Refer to 'Offset calibration function' on page 9
 * of the datasheet for more information.
 *
 * @return 0 on success.
 */
int8_t offset_calibration(void)
{
	int8_t status;
	HSCDTD_CTRL3_t reg;

	status = read_register(HSCDTD_REG_CTRL3, &reg);
	if (status != 0)
		return status;
	
	reg.OCL = 1;

	status = write_register(HSCDTD_REG_CTRL3, &reg);
	if (status != 0)
		return status;

	// TODO(bob): Figure out if bit needs to set back.

	return 0;
}


/**
 * @brief Starts temperature compenstation.
 *
 * Reads temperature and calibrates sensor values
 * based on measured temperature.
 *
 * Must be called explicitly each time temperature
 * compensation is required. The temperature measured,
 * is used for all future compensation, even if the
 * temperature changes.
 *
 * Device must be in the force state.
 *
 * Refer to 'Temperature Measurement and Compensation Function'
 * on page 9 of the datasheet for more information.
 *
 * @return 0 on success.
 */
int8_t temperature_compensation(void)
{
	int8_t status;
	int8_t i;
	HSCDTD_CTRL3_t reg;
	HSCDTD_STAT_t stat;

	status = read_register(HSCDTD_REG_CTRL3, &reg);
	if (status != 0)
		return status;

	reg.TCS = 1;

	status = write_register(HSCDTD_REG_CTRL3, &reg);
	if (status != 0)
		return status;
	
	status = -1;
	// Attempt to check status for ~50ms (Duration does not really matter).
	// If no temperature after that, something has gone wrong.
	for (i = 0; i < 50; i++) {
		t_sleep_ms(1);

		// Read status register to check if temp data is ready.
		status = read_register(HSCDTD_REG_STATUS, &stat);
		if (status != 0)
			return status;
		
		if (stat.TRDY == 1) {
			// The datasheet specifies that the bit is cleared after
			// reading the TEMP register.
			// We don't need the value here, so we don't need the return
			// value.
			read_temp();
			status = 0;
			break;
		}
	}

	return status;
}

/**
 * @brief Gets formatted temperature.
 *
 * This function reads the temperature that is stored
 * in the 'TEMP' register of the device.
 *
 * This function does not start a temperature reading.
 *
 * @return Temperature as signed integer.
 */
int8_t read_temp(void)
{
	int8_t temp;

	// We can safely cast the uint8_t to a int8_t as the the value of the
	// value is formatted as int8_t.
	read_register(HSCDTD_REG_TEMP, &temp);
	// Ignore read register status.

	return temp;
}



/**
 * @brief Perform a selftest on the chip.
 *
 * Refer to 'Selftest' on page 6 of the datasheet for more information.
 *
 * @return 0 on success.
 */
int8_t self_test(void)
{
	int8_t status;
	HSCDTD_CTRL3_t reg;
	uint8_t self_test_resp;

	status = read_register(HSCDTD_REG_CTRL3, &reg);
	if (status != 0)
		return status;

	reg.STC = 1;

	status = write_register(HSCDTD_REG_CTRL3, &reg);
	if (status != 0)
		return status;
	
	// Wait a bit for the result.
	// This is not specified in the datasheet, but just to be safe.
	t_sleep_ms(5);

	// According to page 6 of the datasheet value of STB should be 0xAA at
	// first read.
	status = read_register(HSCDTD_REG_SELFTEST_RESP, &self_test_resp);
	if (status != 0)
		return status;
	
	if (self_test_resp != 0xAA)
		return -20;  //TODO(bob): Replace with better error code
	
	// After reading again value should be 0x55.
	status = read_register(HSCDTD_REG_SELFTEST_RESP, &self_test_resp);
	if (status != 0)
		return status;
	
	if (self_test_resp != 0x55)
		return -30;  //TODO(bob): Replace with better error code

	// If all those test passed, the test is successful.
	return 0;
}


/**
 * @brief Soft reset the device.
 *
 * Soft resetting the device also puts the device back into the
 * stand-by mode.
 *
 * Device must be reconfigured after soft reset.
 *
 * @return 0 on success.
 */
int8_t soft_reset(void)
{
	int8_t status;
	HSCDTD_CTRL3_t reg;

	// The intention is to reset the device and its registers.
	// So there is no need to first read the content of the
	// register.
	reg.SRST = 1;
	status = write_register(HSCDTD_REG_CTRL3, &reg);
	if (status != 0)
		return status;

	t_sleep_ms(5);  // Wait a bit for the chip to reset.

	// Check if the reset went OK
	status = read_register(HSCDTD_REG_CTRL3, &reg);
	if (status != 0)
		return status;

	if (reg.SRST == 1) {
		// If bit is set, something went wrong
		return -1;
	}
	// Reset was successful
	return 0;
}


/**
 * @brief Start a measurement in the force state.
 *
 * @param p_mag_data A pointer to a struct to store the data.
 * @return 0 on success.
 */
int8_t measure(hscdtd_mag_t *p_mag_data)
{
	int8_t status;
	HSCDTD_STAT_t stat;
	HSCDTD_CTRL3_t ctrl3;
	int8_t i;

	// Read the status register to clear any status bits.
	status = read_register(HSCDTD_REG_STATUS, &stat);
	// 'status' is the return value of the register read function, not the
	// content of the register.
	if (status != 0)
		return status;

	// Start measurement
	status = read_register(HSCDTD_REG_CTRL3, &ctrl3);
	if (status != 0)
		return status;
	
	ctrl3.FRC = 1;

	status = write_register(HSCDTD_REG_CTRL3, &ctrl3);
	if (status != 0)
		return status;
	
	// Wait until data is ready.
	for (i = 0; i < 50; i++) {
		status = read_register(HSCDTD_REG_STATUS, &stat);
		if (status != 0)
			return status;
		
		if (stat.DRDY == 1)
			break;

		t_sleep_ms(1);
	}

	if (stat.DRDY != 1)
		return -5;  //TODO(bob): Replace with better error codes.

	// Use magneto read function to read the data into the pointer.
	status = read_magnetodata(p_mag_data);
	return status;
}


int8_t read_magnetodata(hscdtd_mag_t *p_mag_data)
{
	int8_t status;
	int8_t i;
	uint8_t buf[6];
	int16_t tmp;
	float *mag_data = &p_mag_data->mag_x;

	// Read all mag data registers in one go.
	status = t_read_register(HSCDTD_ADDR, HSCDTD_REG_XOUT_L, 6, buf);
	if (status != 0)
		return status;

	for (i = 0; i < HSCDTD_NUM_AXIS; i++) {
		// Each axis is formatted little endian, flip it and make it signed.
		tmp = (int16_t) ((uint16_t)((buf[2 * i + 1] << 8) | (buf[2 * i])));
		mag_data[i] = tmp * HSCDTD_UT_PER_LSB_15B;  // Assumes 15 bit value.
	}

	return 0;
}