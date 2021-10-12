#ifndef __BLE_SBM_H__
#define __BLE_SBM_H__


/**@brief   Macro for defining a ble_sbm instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_SBM_DEF(_name)                                                                          \
static ble_sbm_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_HRS_BLE_OBSERVER_PRIO,                                                     \
                     ble_sbm_on_ble_evt, &_name)

#define SBM_BASE_UUID         {0xBC, 0x8A, 0xBF, 0x45, 0xCA, 0x05, 0x50, 0xBA, \
                                          0x40, 0x42, 0xB0, 0x00, 0xC9, 0xAD, 0x64, 0xF3}
#define SBM_SERVICE_UUID      0x1400
#define SBM_VALUE_UUID        0x1401

/**@brief Custom Service event type. */
typedef enum
{
    BLE_SBM_EVT_NOTIFICATION_ENABLED,                             /**< Custom value notification enabled event. */
    BLE_SBM_EVT_NOTIFICATION_DISABLED,                            /**< Custom value notification disabled event. */
    BLE_SBM_EVT_DISCONNECTED,
    BLE_SBM_EVT_CONNECTED
} ble_sbm_evt_type_t;

/**@brief Custom Service event. */
typedef struct
{
    ble_sbm_evt_type_t evt_type;                                  /**< Type of event. */
} ble_sbm_evt_t;

// Forward declaration of the ble_cus_t type.
typedef struct ble_sbm_s ble_sbm_t;

/**@brief Custom Service event handler type. */
typedef void (*ble_sbm_evt_handler_t) (ble_sbm_t * p_sbm, ble_sbm_evt_t * p_evt);

/**@brief Custom Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_sbm_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t                       initial_custom_value;           /**< Initial custom value */
    ble_srv_cccd_security_mode_t  custom_value_char_attr_md;     /**< Initial security level for Custom characteristics attribute */
} ble_sbm_init_t;


/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_sbm_s
{
    ble_sbm_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t                      service_handle;                 /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t      custom_value_handles;           /**< Handles related to the Custom Value characteristic. */
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                       uuid_type; 
};


/**@brief Function for initializing the Custom Service.
 *
 * @param[out]  p_sbm       Custom Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_sbm_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_sbm_init(ble_sbm_t * p_sbm, const ble_sbm_init_t * p_sbm_init);

/* This code belongs in ble_sbm.c*/

/**@brief Function for adding the Custom Value characteristic.
 *
 * @param[in]   p_sbm        Custom Service structure.
 * @param[in]   p_sbm_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t custom_value_char_add(ble_sbm_t * p_sbm, const ble_sbm_init_t * p_sbm_init);

/* This code belongs in ble_sbm.h*/

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Battery Service.
 *
 * @note 
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 * @param[in]   p_context  Custom Service structure.
 */
void ble_sbm_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);
static void on_connect(ble_sbm_t * p_sbm, ble_evt_t const * p_ble_evt);
static void on_disconnect(ble_sbm_t * p_sbm, ble_evt_t const * p_ble_evt);

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_sbm       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_sbm_t * p_sbm, ble_evt_t const * p_ble_evt);

/**@brief Function for updating the custom value.
 *
 * @details The application calls this function when the cutom value should be updated. If
 *          notification has been enabled, the custom value characteristic is sent to the client.
 *
 * @note 
 *       
 * @param[in]   p_sbm          Custom Service structure.
 * @param[in]   Custom value 
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_sbm_custom_value_update(ble_sbm_t * p_sbm, uint8_t custom_value);

#endif