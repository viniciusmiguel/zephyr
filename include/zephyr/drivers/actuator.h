/*
 * Copyright (c) 2024 Vinicius Miguel
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public Actuator Driver APIs
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_ACTUATOR_H_
#define ZEPHYR_INCLUDE_DRIVERS_ACTUATOR_H_

/**
 * @brief Actuator Interface
 * @defgroup actuator_interface Actuator Interface
 * @since 3.8
 * @version 0.0.1
 * @ingroup io_interfaces
 * @{
 */

#include <errno.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys_clock.h>
#include <zephyr/sys/math_extras.h>
#include <zephyr/toolchain.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Representation of an actuator readout value.
 *
 * The value is represented as having an integer and a fractional part,
 * and can be obtained using the formula val1 + val2 * 10^(-6). Negative
 * values also adhere to the above formula, but may need special attention.
 * Here are some examples of the value representation:
 *
 *      0.5: val1 =  0, val2 =  500000
 *     -0.5: val1 =  0, val2 = -500000
 *     -1.0: val1 = -1, val2 =  0
 *     -1.5: val1 = -1, val2 = -500000
 */
struct actuator_value {
	/** Integer part of the value. */
	int32_t integer_value;
	/** Fractional part of the value (in one-millionth parts). */
	int32_t factional_value;
};

enum actuator_setting {
	/** Proprotional constant in the torque control loop */
	ACTUATOR_SETTING_TORQUE_KP,
	/** Integral constant in the torque control loop */
	ACTUATOR_SETTING_TORQUE_KI,
	/** Derivative constant in the torque control loop */
	ACTUATOR_SETTING_TORQUE_KD,
	/** Integrator limit in the torque control loop */
	ACTUATOR_SETTING_TORQUE_INTEGRATOR_LIMIT,
	/** Maximum output value in the torque control loop */
	ACTUATOR_SETTING_TORQUE_MAX_OUTPUT_VALUE,
	/** Proprotional constant in the velocity control loop */
	ACTUATOR_SETTING_VELOCITY_KP,
	/** Integral constant in the velocity control loop */
	ACTUATOR_SETTING_VELOCITY_KI,
	/** Derivative constant in the velocity control loop */
	ACTUATOR_SETTING_VELOCITY_KD,
	/** Integrator limit in the velocity control loop */
	ACTUATOR_SETTING_VELOCITY_INTEGRATOR_LIMIT,
	/** Maximum output value in the velocity control loop */
	ACTUATOR_SETTING_VELOCITY_MAX_OUTPUT_VALUE,
	/** Proprotional constant in the velocity control loop */
	ACTUATOR_SETTING_POSITION_KP,
	/** Integral constant in the velocity control loop */
	ACTUATOR_SETTING_POSITION_KI,
	/** Derivative constant in the velocity control loop */
	ACTUATOR_SETTING_POSITION_KD,
	/** Integrator limit in the position control loop */
	ACTUATOR_SETTING_POSITION_INTEGRATOR_LIMIT,
	/** Maximum output value in the position control loop */
	ACTUATOR_SETTING_POSITION_MAX_OUTPUT_VALUE,
	/** Number of pole pairs the actuator motor has */
	ACTUATOR_SETTING_MOTOR_POLES,

	/**
	 * Number of all common actuator settings.
	 */
	ACTUATOR_SETTING_COMMON_COUNT,

	/**
	 * This and higher values are actuator specific.
	 * Refer to the actuator header file.
	 */
	ACTUATOR_SETTING_PRIV_START = ACTUATOR_SETTING_COMMON_COUNT,

	/**
	 * Maximum value describing an actuator setting.
	 */
	ACTUATOR_SETTING_MAX = INT16_MAX,
};

enum actuator_control_mode {
	ACTUATOR_TORQUE_CONTROL,
	ACTUATOR_VELOCITY_CONTROL,
	ACTUATOR_POSITION_CONTROL
};

/**
 * @typedef actuator_control_mode_set_t
 * @brief Callback API upon setting an actuator control opperation mode
 *
 * See actuator_control_mode_set() for argument description
 */
typedef int (*actuator_control_mode_set_t)(const struct device *dev,
					   enum actuator_control_mode mode);

/**
 * @typedef actuator_enable_t
 * @brief Callback API eanbling the actuator
 *
 * See actuator_enable() for argument description
 */
typedef int (*actuator_enable_t)(const struct device *dev);

/**
 * @typedef actuator_disable_t
 * @brief Callback API eanbling the actuator
 *
 * See actuator_disable() for argument description
 */

/** @brief Actuator driver API definition. */
typedef int (*actuator_disable_t)(const struct device *dev);

/**
 * @typedef actuator_setting_set_t
 * @brief Callback API upon setting an actuator's setting
 *
 * See actuator_setting_set() for argument description
 */
typedef int (*actuator_setting_set_t)(const struct device *dev, enum actuator_setting sett,
				      const struct actuator_value *val);

/**
 * @typedef actuator_setting_get_t
 * @brief Callback API upon getting an actuator's setting
 *
 * See actuator_setting_get() for argument description
 */
typedef int (*actuator_setting_get_t)(const struct device *dev, enum actuator_setting sett,
				      struct actuator_value *val);

/**
 * @typedef actuator_torque_target_set_t
 * @brief Callback API upon setting an actuator's torque target
 *
 * See actuator_torque_target_set() for argument description
 */
typedef int (*actuator_torque_target_set_t)(const struct device *dev,
					    const struct actuator_value *val);

/**
 * @typedef actuator_torque_target_get_t
 * @brief Callback API upon getting an actuator's torque target
 *
 * See actuator_torque_target_get() for argument description
 */
typedef int (*actuator_torque_target_get_t)(const struct device *dev, struct actuator_value *val);

/**
 * @typedef actuator_velocity_target_set_t
 * @brief Callback API upon setting an actuator's velocity target
 *
 * See actuator_velocity_target_set() for argument description
 */
typedef int (*actuator_velocity_target_set_t)(const struct device *dev,
					      const struct actuator_value *val);

/**
 * @typedef actuator_velocity_target_get_t
 * @brief Callback API upon getting an actuator's velocity target
 *
 * See actuator_velocity_target_get() for argument description
 */
typedef int (*actuator_velocity_target_get_t)(const struct device *dev, struct actuator_value *val);

/**
 * @typedef actuator_position_target_set_t
 * @brief Callback API upon setting an actuator's position target
 *
 * See actuator_position_target_set() for argument description
 */
typedef int (*actuator_position_target_set_t)(const struct device *dev,
					      const struct actuator_value *val);

/**
 * @typedef actuator_position_target_get_t
 * @brief Callback API upon getting an actuator's position target
 *
 * See actuator_position_target_get() for argument description
 */
typedef int (*actuator_position_target_get_t)(const struct device *dev, struct actuator_value *val);

/**
 * @typedef actuator_torque_actual_get_t
 * @brief Callback API upon getting an actuator's actual torque
 *
 * See actuator_torque_actual_get() for argument description
 */
typedef int (*actuator_torque_actual_get_t)(const struct device *dev, struct actuator_value *val);

/**
 * @typedef actuator_velocity_actual_get_t
 * @brief Callback API upon getting an actuator's actual velocity
 *
 * See actuator_velocity_actual_get() for argument description
 */
typedef int (*actuator_velocity_actual_get_t)(const struct device *dev, struct actuator_value *val);

/**
 * @typedef actuator_position_actual_get_t
 * @brief Callback API upon getting an actuator's actual position
 *
 * See actuator_position_actual_get() for argument description
 */
typedef int (*actuator_position_actual_get_t)(const struct device *dev, struct actuator_value *val);

__subsystem struct actuator_driver_api {
	actuator_control_mode_set_t control_mode_set;
	actuator_enable_t enable;
	actuator_disable_t disable;
	actuator_setting_set_t setting_set;
	actuator_setting_get_t setting_get;
	actuator_torque_target_set_t torque_target_set;
	actuator_torque_target_get_t torque_target_get;
	actuator_velocity_target_set_t velocity_target_set;
	actuator_velocity_target_get_t velocity_target_get;
	actuator_position_target_set_t position_target_set;
	actuator_position_target_get_t position_target_get;
	actuator_torque_actual_get_t torque_actual_get;
	actuator_velocity_actual_get_t velocity_actual_get;
	actuator_position_actual_get_t position_actual_get;
};

/**
 * @brief Set the control mode for an actuator
 *
 * @param dev Pointer to the motor device
 * @param mode The control mode to set the motor to (torque, velocity, position)
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_control_mode_set(const struct device *dev, enum actuator_control_mode mode);

static inline int z_impl_actuator_control_mode_set(const struct device *dev,
						   enum actuator_control_mode mode)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->control_mode_set == NULL) {
		return -ENOSYS;
	}

	return api->control_mode_set(dev, mode);
}

/**
 * @brief Enables an actuator
 *
 * @param dev Pointer to the motor device
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_enable(const struct device *dev);

static inline int z_impl_actuator_enable(const struct device *dev)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->enable == NULL) {
		return -ENOSYS;
	}

	return api->enable(dev);
}

/**
 * @brief Disables an actuator
 *
 * @param dev Pointer to the motor device
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_disable(const struct device *dev);

static inline int z_impl_actuator_disable(const struct device *dev)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->disable == NULL) {
		return -ENOSYS;
	}

	return api->disable(dev);
}

/**
 * @brief Set a setting for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param sett The setting to set
 * @param val The value to set the setting to
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_setting_set(const struct device *dev, enum actuator_setting sett,
				   const struct actuator_value *val);

static inline int z_impl_actuator_setting_set(const struct device *dev, enum actuator_setting sett,
					      const struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->setting_set == NULL) {
		return -ENOSYS;
	}

	return api->setting_set(dev, sett, val);
}

/**
 * @brief Get a setting for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param sett The setting to get
 * @param val Pointer to where to store the setting
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_setting_get(const struct device *dev, enum actuator_setting sett,
				   struct actuator_value *val);

static inline int z_impl_actuator_setting_get(const struct device *dev, enum actuator_setting sett,
					      struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->setting_get == NULL) {
		return -ENOSYS;
	}

	return api->setting_get(dev, sett, val);
}

/**
 * @brief Set the torque target for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val The value to set the torque target to
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_torque_target_set(const struct device *dev,
					 const struct actuator_value *val);

static inline int z_impl_actuator_torque_target_set(const struct device *dev,
						    const struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->torque_target_set == NULL) {
		return -ENOSYS;
	}

	return api->torque_target_set(dev, val);
}

/**
 * @brief Get the torque target for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val Pointer to where to store the torque target
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_torque_target_get(const struct device *dev, struct actuator_value *val);

static inline int z_impl_actuator_torque_target_get(const struct device *dev,
						    struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->torque_target_get == NULL) {
		return -ENOSYS;
	}

	return api->torque_target_get(dev, val);
}

/**
 * @brief Set the velocity target for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val The value to set the velocity target to
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_velocity_target_set(const struct device *dev,
					   const struct actuator_value *val);

static inline int z_impl_actuator_velocity_target_set(const struct device *dev,
						      const struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->velocity_target_set == NULL) {
		return -ENOSYS;
	}

	return api->velocity_target_set(dev, val);
}

/**
 * @brief Get the velocity target for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val Pointer to where to store the velocity target
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_velocity_target_get(const struct device *dev, struct actuator_value *val);

static inline int z_impl_actuator_velocity_target_get(const struct device *dev,
						      struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->velocity_target_get == NULL) {
		return -ENOSYS;
	}

	return api->velocity_target_get(dev, val);
}

/**
 * @brief Set the position target for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val The value to set the position target to
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_position_target_set(const struct device *dev,
					   const struct actuator_value *val);

static inline int z_impl_actuator_position_target_set(const struct device *dev,
						      const struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->position_target_set == NULL) {
		return -ENOSYS;
	}

	return api->position_target_set(dev, val);
}

/**
 * @brief Get the position target for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val Pointer to where to store the position target
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_position_target_get(const struct device *dev, struct actuator_value *val);

static inline int z_impl_actuator_position_target_get(const struct device *dev,
						      struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->position_target_get == NULL) {
		return -ENOSYS;
	}

	return api->position_target_get(dev, val);
}

/**
 * @brief Get the actual torque for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val Pointer to where to store the actual torque
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_torque_actual_get(const struct device *dev, struct actuator_value *val);

static inline int z_impl_actuator_torque_actual_get(const struct device *dev,
						    struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->torque_actual_get == NULL) {
		return -ENOSYS;
	}

	return api->torque_actual_get(dev, val);
}

/**
 * @brief Get the actual velocity for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val Pointer to where to store the actual velocity
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_velocity_actual_get(const struct device *dev, struct actuator_value *val);

static inline int z_impl_actuator_velocity_actual_get(const struct device *dev,
						      struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->velocity_actual_get == NULL) {
		return -ENOSYS;
	}

	return api->velocity_actual_get(dev, val);
}

/**
 * @brief Get the actual position for an actuator
 *
 * @param dev Pointer to the actuator device
 * @param val Pointer to where to store the actual position
 *
 * @return 0 if successful, negative errno code if failure.
 */
__syscall int actuator_position_actual_get(const struct device *dev, struct actuator_value *val);

static inline int z_impl_actuator_position_actual_get(const struct device *dev,
						      struct actuator_value *val)
{
	const struct actuator_driver_api *api = (const struct actuator_driver_api *)dev->api;

	if (api->position_actual_get == NULL) {
		return -ENOSYS;
	}

	return api->position_actual_get(dev, val);
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_DRIVERS_ACTUATOR_H_ */