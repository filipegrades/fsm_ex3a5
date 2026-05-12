#include "driverlib.h"
#include "device.h"
#include "board.h"


// 
// Enumeração para o Estado do sistema
typedef enum {
    SYSTEM_STATE_STANDBY,
    SYSTEM_STATE_OPERATING,
    SYSTEM_STATE_FAULT,
    SYSTEM_STATE_RECOVERING
} SystemState_t;

// Contador de recuperação
static unsigned int g_recoveringCounter = 0;

// Variavel que define o numero de ciclos para RECOVERING
unsigned int g_recoveringMinimumValue = 10;

// Enumeração para determinação do estado do LED
typedef enum {
    LED_LIGADO,
    LED_DESLIGADO
} LedState_t;

// Protótipo da função
void system_state_handler(SystemState_t *pState);

// Variavel do estado do sistema
SystemState_t g_systemState1;

//
// Main
//
void main(void)
{
    // Device Initialization
    Device_init();

    //
    // Initializes PIE and clears PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initializes the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

	Board_init();

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    

    while(1)
    {
        system_state_handler(&g_systemState1);
    }	
	
}


// Implementação da função
void system_state_handler(SystemState_t *pState)
{
    switch (*pState) {
        case SYSTEM_STATE_STANDBY:
            GPIO_writePin(LED_azul_GPIO,LED_DESLIGADO);
            GPIO_writePin(LED_vermelho_GPIO,LED_DESLIGADO);
            break;
        case SYSTEM_STATE_OPERATING:
            GPIO_writePin(LED_azul_GPIO,LED_LIGADO);
            GPIO_writePin(LED_vermelho_GPIO,LED_DESLIGADO);
            break;
        case SYSTEM_STATE_FAULT:
            GPIO_writePin(LED_azul_GPIO,LED_LIGADO);
            GPIO_writePin(LED_vermelho_GPIO,LED_LIGADO);
            g_recoveringCounter = g_recoveringCounter + 1;
            break;
        case SYSTEM_STATE_RECOVERING:
        if (g_recoveringCounter < g_recoveringMinimumValue) {
            g_systemState1 = SYSTEM_STATE_OPERATING;
        }
        else {
            g_systemState1 = SYSTEM_STATE_RECOVERING;
        }
            break;
        }
}

__interrupt void INT_Recovering_LED_Timer_ISR(void)
{
    if(g_systemState1 == SYSTEM_STATE_RECOVERING)
    {
        GPIO_togglePin(LED_azul_GPIO);
        GPIO_writePin(LED_vermelho_GPIO,LED_DESLIGADO);
    }

    Interrupt_clearACKGroup(INT_Recovering_LED_Timer_INTERRUPT_ACK_GROUP);
}

//
// End of File
//
