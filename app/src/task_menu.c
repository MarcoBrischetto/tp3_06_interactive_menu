/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : task_menu.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_menu_attribute.h"
#include "task_menu.h"
#include "task_menu_interface.h"
#include "display.h"

/********************** macros and definitions *******************************/
#define G_TASK_MEN_CNT_INI			0ul
#define G_TASK_MEN_TICK_CNT_INI		0ul

#define DEL_MEN_XX_MIN				0ul
#define DEL_MEN_XX_MED				50ul
#define DEL_MEN_XX_MAX				500ul

/********************** internal data declaration ****************************/
task_menu_dta_t task_menu_dta =
	{DEL_MEN_XX_MIN, ST_MEN_XX_MAIN, EV_MEN_MEN_ACTIVE, false, 1, 1, 1, 1, 1};

#define MENU_DTA_QTY	(sizeof(task_menu_dta)/sizeof(task_menu_dta_t))

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
const char *p_task_menu 		= "Task Menu (Interactive Menu)";
const char *p_task_menu_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_menu_cnt;
volatile uint32_t g_task_menu_tick_cnt;

char menu_fijo[16] = "Enter/Next/Esc";
char menu1[2][16] = {">Motor1",">Motor2"};
char menu2[3][16] = {">Power",">Speed",">Spin"};
char menu3[2][16] = {">ON",">OFF"};
char menu4[10][16]={">0",">1",">2",">3",">4",">5",">6",">7",">8",">9"};
char menu5[2][16]={">LEFT",">RIGHT"};

char string_motor1[2][16]={"ON","OFF"};
char string_motor2[10][16]={"0","1","2","3","4","5","6","7","8","9"};
char string_motor3[2][16]={"L","R"};

char *linea1;
char *linea2;
char aux1[16];
char aux2[16];
motor_t motor[2] = {{1,1,1,1},{2,1,1,1}};

//static void motor_init(motor_t motor);
//static void motor_actualizar(motor_t motor, uint8_t power, uint8_t speed, uint8_t spin);

/********************** external functions definition ************************/
void task_menu_init(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
	task_menu_st_t	state;
	task_menu_ev_t	event;
	bool b_event;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_menu_init), p_task_menu);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_menu), p_task_menu_);

	g_task_menu_cnt = G_TASK_MEN_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_menu_cnt), g_task_menu_cnt);

	init_queue_event_task_menu();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	/* Print out: Task execution FSM */
	state = p_task_menu_dta->state;
	LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

	event = p_task_menu_dta->event;
	LOGGER_LOG("   %s = %lu", GET_NAME(event), (uint32_t)event);

	b_event = p_task_menu_dta->flag;
	LOGGER_LOG("   %s = %s\r\n", GET_NAME(b_event), (b_event ? "true" : "false"));

    displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

    //displayCharPositionWrite(0, 0);
    //displayStringWrite("TdSE Bienvenidos");

	//displayCharPositionWrite(0, 1);
	//SdisplayStringWrite("Test Nro: ");

    // Primera escritura
    sprintf(aux1,"Motor1:%s,%s,%s",string_motor1[motor[0].power-1],string_motor2[motor[0].speed-1],string_motor3[motor[0].spin-1]);
    sprintf(aux2,"Motor2:%s,%s,%s",string_motor1[motor[1].power-1],string_motor2[motor[1].speed-1],string_motor3[motor[1].spin-1]);
    linea1=aux1;
    linea2=aux2;

    displayCharPositionWrite(0, 0);
	displayStringWrite("                ");
	displayCharPositionWrite(0, 1);
	displayStringWrite("                ");

	displayCharPositionWrite(0, 0);
	displayStringWrite(linea1);
	displayCharPositionWrite(0, 1);
	displayStringWrite(linea2);

	HAL_GPIO_WritePin(LED_A_PORT, LED_A_PIN, LED_A_ON);

	g_task_menu_tick_cnt = G_TASK_MEN_TICK_CNT_INI;
}

void task_menu_update(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
	bool b_time_update_required = false;
	char menu_str[4];

	/* Update Task Menu Counter */
	g_task_menu_cnt++;

	/* Protect shared resource (g_task_menu_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
    {
    	g_task_menu_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_menu_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
		{
			g_task_menu_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	/* Update Task Menu Data Pointer */
		p_task_menu_dta = &task_menu_dta;

    	if (DEL_MEN_XX_MIN < p_task_menu_dta->tick)
		{
			p_task_menu_dta->tick--;
		}
		else
		{
			HAL_GPIO_TogglePin(LED_A_PORT, LED_A_PIN);

			//snprintf(menu_str, sizeof(menu_str), "%lu", (g_task_menu_cnt/1000ul));
			//displayCharPositionWrite(10, 1);
			//displayStringWrite(menu_str);

			p_task_menu_dta->tick = DEL_MEN_XX_MAX;

			/*
			if (true == any_event_task_menu())
			{
				p_task_menu_dta->flag = true;
				p_task_menu_dta->event = get_event_task_menu();
			}
			*/

			if(true == any_event_task_menu()){
				p_task_menu_dta->flag = true;
				p_task_menu_dta->event = get_event_task_menu();

			switch (p_task_menu_dta->state)
			{
				case ST_MEN_XX_IDLE:

					if ((true == p_task_menu_dta->flag) && (EV_MEN_MEN_ACTIVE == p_task_menu_dta->event))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_ACTIVE;
					}

					break;

				case ST_MEN_XX_ACTIVE:

					if ((true == p_task_menu_dta->flag) && (EV_MEN_MEN_IDLE == p_task_menu_dta->event))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_IDLE;

					}

					break;

				case ST_MEN_XX_MAIN:
					if ((true == p_task_menu_dta->flag) && (EV_MEN_MEN_ACTIVE == p_task_menu_dta->event))
					{
						linea1 = menu_fijo;
						linea2 = menu1[p_task_menu_dta->motor_id - 1];
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_MENU_1;
					}

					break;

				case ST_MEN_XX_MENU_1:
					if ((true == p_task_menu_dta->flag) && (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event))
					{
						linea1=menu_fijo;
						linea2 = menu2[p_task_menu_dta->motor_opt - 1];
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_MENU_2;
					}
					else if ((true == p_task_menu_dta->flag) && (EV_MEN_ESC_ACTIVE == p_task_menu_dta->event))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_MAIN;
						linea1=aux1;
						linea2=aux2;
					}
					else if((true == p_task_menu_dta->flag) && (EV_MEN_NEX_ACTIVE == p_task_menu_dta->event)){
						p_task_menu_dta->flag = false;
						p_task_menu_dta->motor_id = (p_task_menu_dta->motor_id%2) + 1;
						linea1 = menu_fijo;
						linea2 = menu1[p_task_menu_dta->motor_id - 1];
					}
					break;

				case ST_MEN_XX_MENU_2:
					if ((true == p_task_menu_dta->flag) && (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event))
					{
						p_task_menu_dta->flag = false;

						if(p_task_menu_dta->motor_opt == 1){
							p_task_menu_dta->state = ST_MEN_XX_MENU_POWER;
							linea2 = menu3[p_task_menu_dta->power_opt - 1];
						}
						else if(p_task_menu_dta->motor_opt == 2){
							p_task_menu_dta->state = ST_MEN_XX_MENU_SPEED;
							linea2 = menu4[p_task_menu_dta->speed_opt - 1];
						}
						else if(p_task_menu_dta->motor_opt == 3){
							p_task_menu_dta->state = ST_MEN_XX_MENU_SPIN;
							linea2 = menu5[p_task_menu_dta->spin_opt - 1];
						}

					}
					else if ((true == p_task_menu_dta->flag) && (EV_MEN_ESC_ACTIVE == p_task_menu_dta->event))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_MENU_1;
						linea1 = menu_fijo;
						linea2 = menu1[p_task_menu_dta->motor_id - 1];
					}
					else if((true == p_task_menu_dta->flag) && (EV_MEN_NEX_ACTIVE == p_task_menu_dta->event)){
						p_task_menu_dta->motor_opt = (p_task_menu_dta->motor_opt%3) + 1;
						p_task_menu_dta->flag = false;
						linea2 = menu2[p_task_menu_dta->motor_opt - 1];
					}

					break;

				case ST_MEN_XX_MENU_POWER:

					if((true == p_task_menu_dta->flag) && (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event)){
						motor[p_task_menu_dta->motor_id-1].power = p_task_menu_dta->power_opt;
						sprintf(aux1,"Motor1:%s,%s,%s",string_motor1[motor[0].power-1],string_motor2[motor[0].speed-1],string_motor3[motor[0].spin-1]);
						sprintf(aux2,"Motor2:%s,%s,%s",string_motor1[motor[1].power-1],string_motor2[motor[1].speed-1],string_motor3[motor[1].spin-1]);
					}
					else if((true == p_task_menu_dta->flag) && (EV_MEN_NEX_ACTIVE == p_task_menu_dta->event)){
						p_task_menu_dta->power_opt = (p_task_menu_dta->power_opt%2) + 1;
						p_task_menu_dta->flag = false;
						linea2 = menu3[p_task_menu_dta->power_opt - 1];
					}
					else if ((true == p_task_menu_dta->flag) && (EV_MEN_ESC_ACTIVE == p_task_menu_dta->event))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_MENU_2;
						linea2 = menu2[p_task_menu_dta->motor_opt - 1];

					}

					break;

				case ST_MEN_XX_MENU_SPEED:
					if((true == p_task_menu_dta->flag) && (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event)){
						motor[p_task_menu_dta->motor_id-1].speed = p_task_menu_dta->speed_opt;
						sprintf(aux1,"Motor1:%s,%s,%s",string_motor1[motor[0].power-1],string_motor2[motor[0].speed-1],string_motor3[motor[0].spin-1]);
						sprintf(aux2,"Motor2:%s,%s,%s",string_motor1[motor[1].power-1],string_motor2[motor[1].speed-1],string_motor3[motor[1].spin-1]);

					}
					else if((true == p_task_menu_dta->flag) && (EV_MEN_NEX_ACTIVE == p_task_menu_dta->event)){
						p_task_menu_dta->speed_opt = (p_task_menu_dta->speed_opt%10) + 1;
						p_task_menu_dta->flag = false;
						linea2 = menu4[p_task_menu_dta->speed_opt - 1];
					}
					else if ((true == p_task_menu_dta->flag) && (EV_MEN_ESC_ACTIVE == p_task_menu_dta->event))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_MENU_2;
						linea2 = menu2[p_task_menu_dta->motor_opt - 1];
					}

					break;

				case ST_MEN_XX_MENU_SPIN:
					if((true == p_task_menu_dta->flag) && (EV_MEN_ENT_ACTIVE == p_task_menu_dta->event)){
						motor[p_task_menu_dta->motor_id-1].spin = p_task_menu_dta->spin_opt;
						sprintf(aux1,"Motor1:%s,%s,%s",string_motor1[motor[0].power-1],string_motor2[motor[0].speed-1],string_motor3[motor[0].spin-1]);
						sprintf(aux2,"Motor2:%s,%s,%s",string_motor1[motor[1].power-1],string_motor2[motor[1].speed-1],string_motor3[motor[1].spin-1]);

					}
					else if((true == p_task_menu_dta->flag) && (EV_MEN_NEX_ACTIVE == p_task_menu_dta->event)){
						p_task_menu_dta->spin_opt = (p_task_menu_dta->spin_opt%2) + 1;
						p_task_menu_dta->flag = false;

						linea2 = menu5[p_task_menu_dta->spin_opt - 1];
					}
					else if ((true == p_task_menu_dta->flag) && (EV_MEN_ESC_ACTIVE == p_task_menu_dta->event))
					{
						p_task_menu_dta->flag = false;
						p_task_menu_dta->state = ST_MEN_XX_MENU_2;
						linea2 = menu2[p_task_menu_dta->motor_opt - 1];
					}

					break;

				default:

					p_task_menu_dta->tick  = DEL_MEN_XX_MIN;
					p_task_menu_dta->state = ST_MEN_XX_IDLE;
					p_task_menu_dta->event = EV_MEN_MEN_IDLE;
					p_task_menu_dta->flag  = false;

					break;
			}
			displayCharPositionWrite(0, 0);
			displayStringWrite("                ");
			displayCharPositionWrite(0, 1);
			displayStringWrite("                ");

			displayCharPositionWrite(0, 0);
			displayStringWrite(linea1);
			displayCharPositionWrite(0, 1);
			displayStringWrite(linea2);
		}
		}
	}
}




/********************** end of file ******************************************/
