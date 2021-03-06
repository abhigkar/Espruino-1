/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#ifndef NRF_DRV_USBD_H__
#define NRF_DRV_USBD_H__

#include "nrf_drv_common.h"
#include "sdk_errors.h"
#include "nrf_usbd.h"
#include <stdint.h>
#include <stdbool.h>
#include "app_util.h"
/**
 * @defgroup nrf_drv_usbd USB Device raw IP HAL and driver
 * @ingroup nrf_drivers
 * @brief @tagAPI52840 USB Device APIs.
 * @details The USB Device  HAL provides basic APIs for accessing
 * the registers of the USBD.
 * The USB Device driver provides APIs on a higher level.
 */

/**
 * @ingroup  nrf_drv_usbd
 * @defgroup nrf_usbdraw_drv USB Device raw IP driver
 *
 * @brief @tagAPI52840 USB Device raw IP driver.
 * @{
 */

/**
 * @name Possible schemes of DMA scheduling
 *
 * Definition of available configuration constants used by DMA scheduler
 * @{
 */
    /**
     * @brief Highly prioritized access
     *
     * Endpoint with lower number has always higher priority and its data would
     * be transfered first.
     * OUT endpoints ale processed before IN endpoints
     */
    #define NRF_DRV_USBD_DMASCHEDULER_PRIORITIZED 0

    /**
     * @brief Round robin scheme
     *
     * All endpoints are processed in round-robin scheme.
     * It means that when one endpoint is processed next in order would be
     * the nearest with lower number.
     * When no endpoints with lower number requires processing - then
     * all endpoints from 0 are tested.
     */
    #define NRF_DRV_USBD_DMASCHEDULER_ROUNDROBIN  1

/** @} */

/**
 * @brief Number of bytes in the endpoint
 *
 * Constant that informs about endpoint size
 */
#define NRF_DRV_USBD_EPSIZE 64

/**
 * @brief Number of bytes for isochronous endpoints
 *
 * Number of bytes for isochronous endpoints in total.
 * This number would be shared between IN and OUT endpoint.
 * It may be also assigned totaly to one endpoint.
 * @sa nrf_usbd_isosplit_set
 * @sa nrf_usbd_isosplit_get
 */
#define NRF_DRV_USBD_ISOSIZE 1024

/**
 * @name Macros for creating endpoint identifiers
 *
 * Auxiliary macros to be used to create Endpoint identifier that is compatible
 * with USB specification.
 * @{
 */

    /**
     * @brief Create identifier for IN endpoint
     *
     * Simple macro to create IN endpoint identifier for given endpoint number.
     *
     * @param[in] n Endpoint number.
     *
     * @return Endpoint identifier that connects endpoint number and endpoint direction.
     */
    #define NRF_DRV_USBD_EPIN(n)   ((nrf_drv_usbd_ep_t)NRF_USBD_EPIN(n))
    /**
     * @brief Create identifier for OUT endpoint
     *
     * Simple macro to create OUT endpoint identifier for given endpoint number.
     *
     * @param[in] n Endpoint number.
     *
     * @return Endpoint identifier that connects endpoint number and endpoint direction.
     */
    #define NRF_DRV_USBD_EPOUT(n)  ((nrf_drv_usbd_ep_t)NRF_USBD_EPOUT(n))

/** @} */

/**
 * @brief Endpoint identifier
 *
 * Endpoint identifier used in the driver.
 * This endpoint number is consistent with USB 2.0 specification.
 */
typedef enum
{
    NRF_DRV_USBD_EPOUT0 = NRF_USBD_EPOUT(0), /**< Endpoint OUT 0 */
    NRF_DRV_USBD_EPOUT1 = NRF_USBD_EPOUT(1), /**< Endpoint OUT 1 */
    NRF_DRV_USBD_EPOUT2 = NRF_USBD_EPOUT(2), /**< Endpoint OUT 2 */
    NRF_DRV_USBD_EPOUT3 = NRF_USBD_EPOUT(3), /**< Endpoint OUT 3 */
    NRF_DRV_USBD_EPOUT4 = NRF_USBD_EPOUT(4), /**< Endpoint OUT 4 */
    NRF_DRV_USBD_EPOUT5 = NRF_USBD_EPOUT(5), /**< Endpoint OUT 5 */
    NRF_DRV_USBD_EPOUT6 = NRF_USBD_EPOUT(6), /**< Endpoint OUT 6 */
    NRF_DRV_USBD_EPOUT7 = NRF_USBD_EPOUT(7), /**< Endpoint OUT 7 */
    NRF_DRV_USBD_EPOUT8 = NRF_USBD_EPOUT(8), /**< Endpoint OUT 8 */

    NRF_DRV_USBD_EPIN0  = NRF_USBD_EPIN(0), /**< Endpoint IN 0 */
    NRF_DRV_USBD_EPIN1  = NRF_USBD_EPIN(1), /**< Endpoint IN 1 */
    NRF_DRV_USBD_EPIN2  = NRF_USBD_EPIN(2), /**< Endpoint IN 2 */
    NRF_DRV_USBD_EPIN3  = NRF_USBD_EPIN(3), /**< Endpoint IN 3 */
    NRF_DRV_USBD_EPIN4  = NRF_USBD_EPIN(4), /**< Endpoint IN 4 */
    NRF_DRV_USBD_EPIN5  = NRF_USBD_EPIN(5), /**< Endpoint IN 5 */
    NRF_DRV_USBD_EPIN6  = NRF_USBD_EPIN(6), /**< Endpoint IN 6 */
    NRF_DRV_USBD_EPIN7  = NRF_USBD_EPIN(7), /**< Endpoint IN 7 */
    NRF_DRV_USBD_EPIN8  = NRF_USBD_EPIN(8), /**< Endpoint IN 8 */
}nrf_drv_usbd_ep_t;

/**
 * @brief Events generated by the library
 *
 * Enumeration of possible events that may be generated by the library.
 */
typedef enum
{
    NRF_DRV_USBD_EVT_SOF,        /**< Start Of Frame event on USB bus detected */
    NRF_DRV_USBD_EVT_RESET,      /**< Reset condition on USB bus detected */
    NRF_DRV_USBD_EVT_SUSPEND,    /**< This device should go to suspend mode now */
    NRF_DRV_USBD_EVT_RESUME,     /**< This device should resume from suspend now */
    NRF_DRV_USBD_EVT_SETUP,      /**< Setup frame received and decoded */
    NRF_DRV_USBD_EVT_EPTRANSFER, /**<
                                  * For Rx (OUT: Host->Device):
                                  * 1. The packet has been received but there is no buffer prepared for transfer already.
                                  * 2. Whole transfer has been finished
                                  *
                                  * For Tx (IN: Device->Host):
                                  * The last packet from requested transfer has been transfered over USB bus and acknowledged
                                  */
    NRF_DRV_USBD_EVT_CNT         /**< Number of defined events */
}nrf_drv_usbd_event_type_t;

/**
 * @brief Possible endpoint error codes
 *
 * Error codes that may be returned with @ref NRF_DRV_USBD_EVT_EPTRANSFER
 */
typedef enum
{
    NRF_USBD_EP_OK,       /**< No error */
    NRF_USBD_EP_WAITING,  /**< Data received, no buffer prepared already - waiting for configured transfer */
    NRF_USBD_EP_OVERLOAD, /**< Received number of bytes cannot fit given buffer
                           *   This error would also be returned when next_transfer function has been defined
                           *   but currently received data cannot fit completely in current buffer.
                           *   No data split from single endpoint transmission is supported.
                           *
                           *   When this error is reported - data is left inside endpoint buffer.
                           *   Clear endpoint or prepare new buffer and read it.
                           */
    NRF_USBD_EP_ABORTED,  /**< EP0 transfer can be aborted when new setup comes.
                           *   Any other transfer can be aborted by USB reset or library stopping.
                           */
}nrf_drv_usbd_ep_status_t;


/**
 * @brief Event structure
 *
 * Structure passed to event handler
 */
typedef struct
{
    nrf_drv_usbd_event_type_t type;
    union
    {
        struct{
            uint16_t framecnt; //!< Current value of frame counter
        }sof; //!< Data aviable for @ref NRF_DRV_USBD_EVT_SOF
        struct{
            nrf_drv_usbd_ep_t        ep;     //!< Endpoint number
        }isocrc;
        struct{
            nrf_drv_usbd_ep_t        ep;     //!< Endpoint number
            nrf_drv_usbd_ep_status_t status; //!< Status for the endpoint
        }eptransfer;
    }data;
}nrf_drv_usbd_evt_t;

/**
 * @brief Helper macro to create endpoint transfer event
 */
#define NRF_DRV_USBD_EP_TRANSFER_EVENT(name, endpont, ep_stat)          \
    nrf_drv_usbd_evt_t name = {                                         \
        NRF_DRV_USBD_EVT_EPTRANSFER,                                    \
        .data = {                                                       \
            .eptransfer = {                                             \
                    .ep = endpont,                                      \
                    .status = ep_stat                                   \
            }                                                           \
        }                                                               \
    }

/**
 * @brief USBD event callback function type.
 *
 * @param[in] p_event Event information structure.
 */
typedef void (*nrf_drv_usbd_event_handler_t)(nrf_drv_usbd_evt_t const * const p_event);

/**
 * @brief The structure to be filled with the information about next transfer
 *
 * When the next transfer is set as a buffer it would get a pointer to this
 * structure to fill it with required transfer data.
 */
typedef struct
{
    union
    {
        void const * tx; //!< Constant tx buffer pointer
        void * rx;       //!< Writable rx buffer pointer
        uint32_t ptr;    //!< Numeric value used internally by the library
    }p_data;             //!< Union with available data pointers used by the library
    size_t size;         //!< Size of the requested transfer
}nrf_drv_usbd_transfer_t;

/**
 * @brief Helper macro for declaring IN transfer item (@ref nrf_drv_usbd_transfer_t)
 *
 * @param name    Instance name
 * @param tx_buff Buffer to transfer
 * @param tx_size Transfer size
 * */
#define NRF_DRV_USBD_TRANSFER_IN(name, tx_buff, tx_size)        \
    nrf_drv_usbd_transfer_t name = {                            \
       .p_data = { .tx = (tx_buff)  },                          \
       .size = (tx_size),                                       \
    }

/**
 * @brief Helper macro for declaring OUT transfer item (@ref nrf_drv_usbd_transfer_t)
 *
 * @param name    Instance name
 * @param rx_buff Buffer to transfer
 * @param rx_size Transfer size
 * */
#define NRF_DRV_USBD_TRANSFER_OUT(name, rx_buff, rx_size)       \
    nrf_drv_usbd_transfer_t name = {                            \
       .p_data = { .rx = (rx_buff)  },                          \
       .size = (rx_size),                                       \
    }

/**
 * @brief USBD event callback for the endpoint
 *
 * @param[out] p_next   Structure with the data for the next transfer to be filled.
 *                      Required only if the function returns true.
 * @param[in]  p_contex Contex variable configured with the endpoint.
 *
 * @retval false Nothing more to transfer
 * @retval true  Settings to the next transfer filled into @c p_next.
 */
typedef bool (*nrf_drv_usbd_next_transfer_handler_t)(nrf_drv_usbd_transfer_t * p_next, void * p_context);

/**
 * @brief USBD transfer callback structure
 *
 * Structure used to setup the callback for any type of the transfer
 */
typedef struct
{
    nrf_drv_usbd_next_transfer_handler_t handler; //!< The event handler to be called when buffer is ready.
    void * p_context;                             //!< Context pointer to be send to every called event.
}nrf_drv_usbd_transfer_handler_desc_t;


/**
 * @brief Setup packet structure
 *
 * Structure that contains interpreted SETUP packet.
 */
typedef struct
{
    uint8_t  bmRequestType; //!< byte 0
    uint8_t  bmRequest;     //!< byte 1
    uint16_t wValue;        //!< byte 2
    uint16_t wIndex;        //!< byte 4, 5
    uint16_t wLength;       //!< byte 6, 7
}nrf_drv_usbd_setup_t;

/**
 * @brief Library initialization
 *
 * @param[in] event_handler Event handler provided by the user.
 */
ret_code_t nrf_drv_usbd_init(nrf_drv_usbd_event_handler_t const event_handler);

/**
 * @brief Library deinitialization
 */
ret_code_t nrf_drv_usbd_uninit(void);

/**
 * @brief Enable the USBD port
 *
 * After calling this function USBD peripheral would be enabled.
 * It means that High Frequency clock would be requested and USB LDO would be enabled.
 *
 * In normal situation this function should be called in reaction to USBDETECTED
 * event from POWER peripheral.
 *
 * Interrupts and USB pins pull-up would stay disabled until @ref nrf_drv_usbd_start
 * function is called.
 */
void nrf_drv_usbd_enable(void);

/**
 * @brief Disable the USBD port
 *
 * After calling this function USBD peripheral would be disabled.
 * No events would be detected or processed by the library.
 * Clock for the peripheral would be disconnected.
 */
void nrf_drv_usbd_disable(void);

/**
 * @brief Start USB functionality
 *
 * After calling this function USBD peripheral should be fully functional
 * and all new incoming events / interrupts would be processed by the library.
 *
 * Also only after calling this function host sees new connected device.
 *
 * Call this function when USBD power LDO regulator is ready - on USBPWRRDY event
 * from POWER peripheral.
 *
 * @param enable_sof The flag that is used to enable SOF processing.
 *                   If it is false, SOF interrupt is left disabled and will not be generated.
 *                   This improves power saving if SOF is not required.
 *
 * @note If the isochronous endpoints are going to be used,
 *       it is required to enable the SOF.
 *       In other case any isochronous endpoint would stay busy
 *       after first transmission.
 */
void nrf_drv_usbd_start(bool enable_sof);

/**
 * @brief Stop USB functionality
 *
 * This function disables USBD pull-up and interrupts.
 *
 * @note
 * This function can also be used to logically disconnect USB from the HOST that
 * would force it to enumerate device after calling @ref nrf_drv_usbd_start.
 */
void nrf_drv_usbd_stop(void);

/**
 * @brief Check if driver is initialized
 *
 * @retval false Driver is not initialized
 * @retval true Driver is initialized
 */
bool nrf_drv_usbd_is_initialized(void);

/**
 * @brief Check if driver is enabled
 *
 * @retval false Driver is disabled
 * @retval true  Driver is enabled
 */
bool nrf_drv_usbd_is_enabled(void);

/**
 * @brief Check if driver is started
 *
 * @retval false Driver is not started
 * @retval true Driver is started (fully functional)
 * @note The USBD peripheral interrupt state is checked
 */
bool nrf_drv_usbd_is_started(void);

/**
 * @brief Configure packet size that should be supported by the endpoint
 *
 * The real endpoint buffer size is always the same.
 * This value sets max packet size that would be transmitted over the endpoint.
 * This is required by the library
 *
 * @param[in] ep   Endpoint number
 * @param[in] size Required maximum packet size
 *
 * @note Endpoint size is always set to @ref NRF_DRV_USBD_EPSIZE or @ref NRF_DRV_USBD_ISOSIZE / 2
 *       when @ref nrf_drv_usbd_ep_enable function is called.
 */
void nrf_drv_usbd_ep_max_packet_size_set(nrf_drv_usbd_ep_t ep, uint16_t size);

/**
 * @brief Get configured endpoint packet size
 *
 * Function to get configured endpoint size on the buffer.
 *
 * @param[in] ep Endpoint number
 *
 * @return Maximum pocket size configured on selected endpoint
 */
uint16_t nrf_drv_usbd_ep_max_packet_size_get(nrf_drv_usbd_ep_t ep);

/**
 * @brief Enable selected endpoint
 *
 * This function enables endpoint itself and its interrupts.
 * @param ep Endpoint number to enable
 *
 * @note
 * Max packet size is set to endpoint default maximum value.
 *
 * @sa nrf_drv_usbd_ep_max_packet_size_set
 */
void nrf_drv_usbd_ep_enable(nrf_drv_usbd_ep_t ep);

/**
 * @brief Disable selected endpoint
 *
 * This function disables endpoint itself and its interrupts.
 * @param ep Endpoint number to disable
 */
void nrf_drv_usbd_ep_disable(nrf_drv_usbd_ep_t ep);

/**
 * @brief Start sending data over endpoint
 *
 * Function initializes endpoint transmission.
 * This is asynchronous function - it finishes immediately after configuration
 * for transmission is prepared.
 *
 * @note Data buffer pointed by p_data have to be kept active till
 *       @ref NRF_DRV_USBD_EVT_EPTRANSFER event is generated.
 *
 * @param[in] ep         Endpoint number.
 *                       For IN endpoint sending would be initiated.
 *                       For OUT endpoint receiving would be initiated.
 * @param[in] p_transfer
 * @param[in] p_handler  Description of transfer handler to be called when buffer is ready.
 *                       Can be NULL when not required by the caller.
 *
 * @retval NRF_ERROR_BUSY          Selected endpoint is pending.
 * @retval NRF_ERROR_INVALID_ADDR  Unexpected transfer on EPIN0 or EPOUT0.
 * @retval NRF_ERROR_FORBIDDEN     Endpoint stalled.
 * @retval NRF_SUCCESS             Transfer queued or started.
 */
ret_code_t nrf_drv_usbd_ep_transfer(
    nrf_drv_usbd_ep_t                                  ep,
    nrf_drv_usbd_transfer_t              const * const p_transfer,
    nrf_drv_usbd_transfer_handler_desc_t const * const p_handler);

/**
 * @brief Get the information about last finished transfer
 *
 * Function returns the status of the last buffer set for transfer on selected endpoint.
 * The status considers last buffer set by @ref nrf_drv_usbd_ep_transfer function or
 * by transfer callback function.
 *
 * @param[in]  ep         Endpoint number.
 * @param[out] p_transfer Structure that would be filled by transfer details: buffer pointer and number of bytes received.
 *
 * @retval NRF_SUCCESS         Transfer already finished
 * @retval NRF_ERROR_BUSY      Ongoing transfer
 * @retval NRF_ERROR_DATA_SIZE Too much of data received that cannot fit into buffer and cannot be splited into chunks.
 *                             This may happen if buffer size is not a multiplication of endpoint buffer size.
 */
ret_code_t nrf_drv_usbd_ep_status_get(nrf_drv_usbd_ep_t ep, nrf_drv_usbd_transfer_t * p_transfer);

/**
 * @brief Get number of received bytes
 *
 * Get the number of received bytes.
 * The function behavior is undefined when called on IN endpoint.
 *
 * @param ep Endpoint number.
 *
 * @return Number of received bytes
 */
size_t nrf_drv_usbd_epout_size_get(nrf_drv_usbd_ep_t ep);

/**
 * @brief Check if endpoint buffer is ready or is under USB IP control
 *
 * Function to test if endpoint is busy.
 * Endpoint that is busy cannot be accessed by MCU.
 * It means that:
 * - OUT (TX) endpoint: Last uploaded data is still in endpoint and is waiting
 *                      to be received by the host.
 * - IN  (RX) endpoint: Endpoint is ready to receive data from the host
 *                      and the endpoint does not have any data.
 * When endpoint is not busy:
 * - OUT (TX) endpoint: New data can be uploaded.
 * - IN  (RX) endpoint: New data can be downloaded using @ref nrf_drv_usbd_ep_transfer
 *                      function.
 */
bool nrf_drv_usbd_ep_is_busy(nrf_drv_usbd_ep_t ep);

/**
 * @brief Stall endpoint
 *
 * Stall endpoit to send error information during next transfer request from
 * the host.
 *
 * @note To stall endpoint it is safer to use @ref nrf_drv_usbd_setup_stall
 * @note Stalled endpoint would not be cleared when DMA transfer finishes.
 *
 * @param ep Endpoint number to stall
 *
 */
void nrf_drv_usbd_ep_stall(nrf_drv_usbd_ep_t ep);

/**
 * @brief Clear stall flag on endpoint
 *
 * This function clears endpoint that is stalled.
 * @note
 * If it is OUT endpoint (receiving) it would be also prepared for reception.
 * It means that busy flag would be set.
 * @note
 * In endpoint (transmitting) would not be cleared - it gives possibility to
 * write new data before transmitting.
 */
void nrf_drv_usbd_ep_stall_clear(nrf_drv_usbd_ep_t ep);

/**
 * @brief Check if endpoint is stalled
 *
 * This function gets stall state of selected endpoint
 *
 * @param ep Endpoint number to check
 */
bool nrf_drv_usbd_ep_stall_check(nrf_drv_usbd_ep_t ep);

/**
 * @brief Get parsed setup data
 *
 * Function fills the parsed setup data structure.
 *
 * @param[out] p_setup Pointer to data structure that would be filled by
 *                     parsed data.
 */
void nrf_drv_usbd_setup_get(nrf_drv_usbd_setup_t * const p_setup);

/**
 * @brief Clear only for data transmission on setup endpoint
 *
 * This function may be called if any more data in control write transfer is expected.
 * Clears only OUT endpoint to be able to take another OUT data token.
 * It does not allow STATUS stage.
 * @sa nrf_drv_usbd_setup_clear
 */
void nrf_drv_usbd_setup_data_clear(void);

/**
 * @brief Clear setup endpoint
 *
 * This function acknowledges setup when SETUP command was received and processed.
 * It has to be called if no data respond for the SETUP command is sent.
 *
 * When there is any data transmission after SETUP command the data transmission
 * itself would clear the endpoint.
 */
void nrf_drv_usbd_setup_clear(void);

/**
 * @brief Stall setup endpoint
 *
 * Mark and error on setup endpoint.
 */
void nrf_drv_usbd_setup_stall(void);

/**
* @note
* This function locks interrupts that may be costly.
* It is good idea to test if the endpoint is still busy before calling this function:
* @code
  (m_ep_dma_waiting & (1U << ep2bit(ep)))
* @endcode
* This function would check it again, but it makes it inside critical section.
*/
void usbd_drv_ep_abort(nrf_drv_usbd_ep_t ep);

/**
 * @brief Get the information about expected transfer SETUP data direction
 *
 * Function returns the information about last expected transfer direction.
 *
 * @retval NRF_DRV_USBD_EPOUT0 Expecting OUT (Host->Device) direction or no data
 * @retval NRF_DRV_USBD_EPIN0  Expecting IN (Device->Host) direction
 */
nrf_drv_usbd_ep_t nrf_drv_usbd_last_setup_dir_get(void);

/**
 * @brief Drop transfer on OUT endpoint
 *
 * @param[in] ep  OUT endpoint ID
 */
void nrf_drv_usbd_transfer_out_drop(nrf_drv_usbd_ep_t ep);

/** @} */
#endif /* NRF_DRV_USBD_H__ */
