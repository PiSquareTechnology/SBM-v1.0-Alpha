
#include "D:\Nordic\nRF5_SDK_17.1.0\examples\Nordic_Projects\SBM-v1.0-Alpha\pca10056\s140\ses\ble_sbm.h"
uint32_t ble_sbm_init(ble_sbm_t * p_sbm, const ble_sbm_init_t * p_sbm_init)
{
    if (p_sbm == NULL || p_sbm_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t   err_code;
    ble_uuid_t ble_uuid;
    // Initialize service structure
    p_sbm->evt_handler               = p_sbm_init->evt_handler;
    p_sbm->conn_handle               = BLE_CONN_HANDLE_INVALID;
    // Initialize service structure
    p_sbm->conn_handle               = BLE_CONN_HANDLE_INVALID;

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = {SBM_BASE_UUID};
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &p_sbm->uuid_type);
    VERIFY_SUCCESS(err_code);
    
    ble_uuid.type = p_sbm->uuid_type;
    ble_uuid.uuid = SBM_SERVICE_UUID;

    // Add the Custom Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_sbm->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Custom Value characteristic
    return custom_value_char_add(p_sbm, p_sbm_init);
}
/* This code belongs in ble_sbm.c*/

/**@brief Function for adding the Custom Value characteristic.
 *
 * @param[in]   p_sbm        Custom Service structure.
 * @param[in]   p_sbm_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t custom_value_char_add(ble_sbm_t * p_sbm, const ble_sbm_init_t * p_sbm_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    //  Read  operation on Cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;
    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_sccd_md         = NULL;
    char_md.p_cccd_md         = &cccd_md; 

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_sbm_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_sbm_init->custom_value_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;


    ble_uuid.type = p_sbm->uuid_type;
    ble_uuid.uuid = SBM_VALUE_UUID;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t);

    err_code = sd_ble_gatts_characteristic_add(p_sbm->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_sbm->custom_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

void ble_sbm_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_sbm_t * p_sbm = (ble_sbm_t *) p_context;
    
    if (p_sbm == NULL || p_ble_evt == NULL)
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_sbm, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_sbm, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE:
            on_write(p_sbm, p_ble_evt);
            break;
        default:
            // No implementation needed.
            break;
    }
}
/* This code belongs in ble_sbm.c*/

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_sbm       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_sbm_t * p_sbm, ble_evt_t const * p_ble_evt)
{
    p_sbm->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    ble_sbm_evt_t evt;

    evt.evt_type = BLE_SBM_EVT_CONNECTED;

    p_sbm->evt_handler(p_sbm, &evt);
}
/* This code belongs in ble_cus.c*/

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_sbm_t * p_sbm, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_sbm->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/* This code belongs in ble_sbm.c*/

static void on_write(ble_sbm_t * p_sbm, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    // Check if the handle passed with the event matches the Custom Value Characteristic handle.
    if (p_evt_write->handle == p_sbm->custom_value_handles.value_handle)
    {
        if( *p_evt_write->data == 2)
        {
          nrf_gpio_pin_toggle(LED_4);
        }
        else if( *p_evt_write->data == 3)
        {
          nrf_gpio_pin_toggle(LED_3);
        }
    }
    // Check if the Custom value CCCD is written to and that the value is the appropriate length, i.e 2 bytes.
    if ((p_evt_write->handle == p_sbm->custom_value_handles.cccd_handle)
        && (p_evt_write->len == 2)
       )
    {
        // CCCD written, call application event handler
        if (p_sbm->evt_handler != NULL)
        {
            ble_sbm_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_SBM_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_SBM_EVT_NOTIFICATION_DISABLED;
            }
            // Call the application event handler.
            p_sbm->evt_handler(p_sbm, &evt);
        }
    }
}

uint32_t ble_sbm_custom_value_update(ble_sbm_t * p_sbm, uint8_t custom_value)
{
    NRF_LOG_INFO("In ble_cus_custom_value_update. \r\n"); 
    if (p_sbm == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = sizeof(uint8_t);
    gatts_value.offset  = 0;
    gatts_value.p_value = &custom_value;

    // Update database.
    err_code = sd_ble_gatts_value_set(p_sbm->conn_handle,
                                      p_sbm->custom_value_handles.value_handle,
                                      &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Send value if connected and notifying.
    if ((p_sbm->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_sbm->custom_value_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len  = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_sbm->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }


    return err_code;
}
#if 0
uint32_t ble_sbm_data_send(ble_sbm_t *p_sbm, uint8_t *p_data, uint16_t *p_length, uint16_t conn_handle)
{
    ret_code_t err_code;
    ble_gatts_hvx_params_t hvx_params;
    ble_sbm_client_context_t * p_client; 
    VERIFY_PARAM_NOT_NULL(p_sbm); 
    err_code = blcm_link_ctx_get(p_sbm->p_link_ctx_storage, conn_handle, (void *) &p_client);
    VERIFY_SUCCESS(err_code); 
    if ((conn_handle == BLE_CONN_HANDLE_INVALID) || (p_client == NULL))
    {
      return NRF_ERROR_NOT_FOUND;
    } 
    if (!p_client->is_notification_enabled)
    {
      return NRF_ERROR_INVALID_STATE;
    } 
    //if (*p_length > BLE_NUS_MAX_DATA_LEN)
    //{
    //  return NRF_ERROR_INVALID_PARAM;
    //} 
    memset(&hvx_params, 0, sizeof(hvx_params)); 
    hvx_params.handle = p_sbm->custom_value_handles.value_handle;
    hvx_params.p_data = p_data;
    hvx_params.p_len = p_length;
    hvx_params.type = BLE_GATT_HVX_NOTIFICATION; 
    return sd_ble_gatts_hvx(conn_handle, &hvx_params);
}

#endif