#include <lpc21xx.h>
#include "types.h"
#include "can_defines.h"
#include "can.h"

/*
 * Function: init_can1()
 * ---------------------
 * Initializes CAN1 peripheral of LPC2129.
 *
 * Steps:
 * 1. Configure the CAN1 TX/RX pins using PINSEL1.
 * 2. Put CAN1 into reset/configuration mode.
 * 3. Configure acceptance filter.
 * 4. Configure CAN baud rate using C1BTR.
 * 5. Exit reset mode and start CAN communication.
 */
void init_can1(void)
{
        /*
         * Clear the previous function selection for P0.25.
         *
         * P0.25 uses bits 18 and 19 of PINSEL1.
         * ~3 clears both bits.
         */
        PINSEL1 &= (u32)~3 << ((25-16)*2);

        /*
         * Select CAN1 function for P0.25.
         *
         * Function select value = 01.
         */
        PINSEL1 |= 1 << ((25-16)*2);

        /*
         * Put CAN1 into reset/configuration mode.
         *
         * C1MOD bit 0 = 1 → Reset mode.
         * CAN configuration should be done in reset mode.
         */
        C1MOD = 1 << 0;

        /*
         * Configure Acceptance Filter Mode Register.
         *
         * Clear bit 0 and set bit 1.
         * This configures the acceptance filter mode
         * used by CAN1.
         */
        AFMR &= ~(1 << 0);
        AFMR = (1 << 1);

        /*
         * Configure CAN1 bit timing.
         *
         * btr_val is defined in can_defines.h.
         * It determines CAN baud rate and timing parameters.
         */
        C1BTR = btr_val;

        /*
         * Exit reset/configuration mode.
         *
         * C1MOD bit 0 = 0 → Normal operating mode.
         */
        C1MOD &= ~(1 << 0);
}


/*
 * Function: can1_tx1()
 * --------------------
 * Transmits one CAN message using CAN1.
 *
 * Parameter:
 *     txf → CAN frame containing:
 *           - CAN ID
 *           - RTR bit
 *           - DLC
 *           - Data bytes
 */
void can1_tx1(canf txf)
{
        /*
         * Check whether CAN1 Transmit Buffer 1 is available.
         *
         * TBS bit = Transmit Buffer Status.
         * 1 → buffer available
         * 0 → buffer busy
         *
         * Wait until the transmit buffer becomes available.
         */
        while(((C1GSR >> tbs_bit) & 1) == 0);

        /*
         * Load the CAN identifier into Transmit ID register.
         */
        C1TID1 = txf.id;

        /*
         * Configure the CAN frame information.
         *
         * RTR → Remote Transmission Request
         * DLC → Data Length Code
         *
         * The values are shifted to their respective
         * positions in the Transmit Frame Information register.
         */
        C1TFI1 = ((txf.bfv.rtr << rtr_bit) |
                  (txf.bfv.dlc << dlc_bit));

        /*
         * If RTR = 0, this is a DATA frame.
         *
         * Therefore, load the actual data into
         * the CAN transmit data registers.
         */
        if(txf.bfv.rtr != 1)
        {
                /*
                 * Load first 4 bytes of CAN data.
                 */
                C1TDA1 = txf.data1;

                /*
                 * Load next 4 bytes of CAN data.
                 */
                C1TDB1 = txf.data2;
        }

        /*
         * Request CAN controller to send the message.
         *
         * stb1_bit → select Transmit Buffer 1.
         * tr_bit   → initiate transmission.
         */
        C1CMR |= ((1 << stb1_bit) |
                  (1 << tr_bit));

        /*
         * Wait until transmission is successfully completed.
         *
         * TCS bit:
         * 1 → transmission completed
         * 0 → transmission still in progress
         */
        while(((C1GSR >> tcs_bit) & 1) == 0);
}


/*
 * Function: can1_rx1()
 * --------------------
 * Receives one CAN message using CAN1.
 *
 * Parameter:
 *     rxf → pointer to CAN frame structure where
 *           received data will be stored.
 */
void can1_rx1(canf *rxf)
{
        /*
         * Check whether a CAN message is available
         * in the Receive Buffer.
         *
         * RBS bit:
         * 1 → receive buffer contains a message
         * 0 → receive buffer is empty
         *
         * Wait until a message is received.
         */
        while(((C1GSR >> rbs_bit) & 1) == 0);

        /*
         * Read the received CAN identifier.
         */
        rxf->id = C1RID;

        /*
         * Read the RTR bit from the Receive Frame
         * Information register.
         */
        rxf->bfv.rtr = ((C1RFS >> rtr_bit) & 1);

        /*
         * Read the Data Length Code.
         *
         * DLC is extracted from C1RFS and masked
         * with 0xF because DLC uses 4 bits.
         */
        rxf->bfv.dlc = ((C1RFS >> dlc_bit) & 15);

        /*
         * If RTR = 0, the received frame is a DATA frame.
         *
         * Read the received CAN data.
         */
        if(rxf->bfv.rtr == 0)
        {
                /*
                 * Read first 4 bytes of received data.
                 */
                rxf->data1 = C1RDA;

                /*
                 * Read next 4 bytes of received data.
                 */
                rxf->data2 = C1RDB;
        }

        /*
         * Release/clear the Receive Buffer.
         *
         * This tells the CAN controller that the
         * received message has been processed.
         */
        C1CMR = 1 << rrb_bit;
}