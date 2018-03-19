/**
 * \file
 * \brief  Microchip CryptoAuth device object
 *
 * \copyright (c) 2017 Microchip Technology Inc. and its subsidiaries.
 *            You may use this software and any derivatives exclusively with
 *            Microchip products.
 *
 * \page License
 *
 * (c) 2017 Microchip Technology Inc. and its subsidiaries. You may use this
 * software and any derivatives exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIPS TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

#include <stdlib.h>
#include <string.h>
#include "atca_device.h"

/** \defgroup device ATCADevice (atca_)
 * \brief ATCADevice object - composite of command and interface objects
   @{ */

/** \brief constructor for an Microchip CryptoAuth device
 * \param[in] cfg  pointer to an interface configuration object
 * \return reference to a new ATCADevice
 */

ATCADevice newATCADevice(ATCAIfaceCfg *cfg)
{
    ATCADevice ca_dev = NULL;

    if (cfg == NULL)
    {
        return NULL;
    }

    ca_dev = (ATCADevice)malloc(sizeof(struct atca_device));
    ca_dev->mCommands = (ATCACommand)newATCACommand(cfg->devtype);
    ca_dev->mIface    = (ATCAIface)newATCAIface(cfg);

    if (ca_dev->mCommands == NULL || ca_dev->mIface == NULL)
    {
        free(ca_dev);
        ca_dev = NULL;
    }

    return ca_dev;
}

/** \brief returns a reference to the ATCACommand object for the device
 * \param[in] dev  reference to a device
 * \return reference to the ATCACommand object for the device
 */
ATCACommand atGetCommands(ATCADevice dev)
{
    return dev->mCommands;
}

/** \brief returns a reference to the ATCAIface interface object for the device
 * \param[in] dev  reference to a device
 * \return reference to the ATCAIface object for the device
 */

ATCAIface atGetIFace(ATCADevice dev)
{
    return dev->mIface;
}

/** \brief destructor for a device NULLs reference after object is freed
 * \param[in] ca_dev  pointer to a reference to a device
 *
 */
void deleteATCADevice(ATCADevice *ca_dev)   // destructor
{
    struct atca_device *dev = *ca_dev;

    if (*ca_dev)
    {
        deleteATCACommand( (ATCACommand*)&(dev->mCommands));
        deleteATCAIface((ATCAIface*)&(dev->mIface));
        free((void*)*ca_dev);
    }

    *ca_dev = NULL;
}

struct DevRevEntry {
    ATCADeviceType devtype;
    uint8_t devrev[4];
};

static struct DevRevEntry devRevTable[] = {
    { .devtype = ATECC608A, .devrev = { 0x00, 0x00, 0x60, 0x01 } },
    { .devtype = ATECC508A, .devrev = { 0x00, 0x00, 0x50, 0x00 } },
    { .devtype = ATECC108A, .devrev = { 0x80, 0x00, 0x10, 0x01 } },
    { .devtype = ATSHA204A, .devrev = { 0x00, 0x02, 0x00, 0x08 } },
    { .devtype = ATSHA204A, .devrev = { 0x00, 0x02, 0x00, 0x09 } },
    { .devtype = ATSHA204A, .devrev = { 0x00, 0x04, 0x05, 0x00 } },
    { .devtype = ATCA_DEV_UNKNOWN },
};

/** \brief Determine the device type by examining the Device Revision data
 * read from the device using the Info command.
 *
 * \param[in] devrev_data  Pointer to 4 byte array of data read from device
 *                         using Info command.
 *
 * \return Returns a value from the ATCADeviceType enum ATCA_DEV_UNKNOWN
 *         if device was not found in the lookup table.
 */
ATCADeviceType atGetDeviceType(const char *devrev_data)
{
    struct DevRevEntry *dre = devRevTable;

    ATCADeviceType devtype = ATCA_DEV_UNKNOWN;

    while (dre->devtype != ATCA_DEV_UNKNOWN)
    {
        if (memcmp(devrev_data, dre->devrev, 4) == 0)
        {
            devtype = dre->devtype;
            break;
        }
    }
    return devtype;
}

/** @} */
