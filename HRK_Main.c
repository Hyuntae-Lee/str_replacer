#include "types.h"
#include "evmam1808.h"
#include "AM1808_gpio.h"
#include "ARM9_subsystem.h"
#include "ArmPheripheralSetup.h"
#include "interrupt.h"
#include "evmam1808_i2c.h"
#include "EMIFA_param.h"
#include "AM1808_thprt.h"
#include "HRK_key.h"

#include "HRK_Timer.h"
#include "HRK_Uart.h"
#include "HRK_Dev.h" 
#include "HRK_Motor.h"
#include "HRK_Disp.h"
#include "HRK_Lcd.h"
#include "HRK_Osd.h"
#include "HRK_Font.h"
#include "HRK_Camera.h"
#include "HRK_Scrn.h"
#include "HRK_FPGA.h"
#include "HRK_Msr.h"
#include "HRK_MsrDisp.h"
#include "HRK_Result.h"
#include "HRK_EngSetup.h"
#include "HRK_Image.h"
#include "HRK_SysV.h"
#include "HRK_Debug.h"
#include "HRK_Wavefront.h"
#include "HRK_Control.h"
#include "HRK_App.h"
#include "HRK_Comm.h"
#include "HRK_DispSize.h"
#include "HRK_DispIllum.h"
#include "HRK_MapView.h"
#include "HRK_Uart.h"
#include "HRK_Rtc.h"
#include "HRK_Focus.h"
#include "HRK_prt_font.h"
#include "HRK_thprt.h"
#include "HRK_Lang.h" 
#include "HRK_UserSetup.h"
#include "HRK_UserSys.h"
#include "HRK_Dpc.h"
#include "MDIN240_main.h"

#include "LM_print.h"

// jig sw 20160223
void	start_jig_mode(void) ;

void main(void)
{
	
	U8			key_code ;
	int			side ;
	BOOL		func_set ;

	BOOL		do_update ;
	BOOL		do_save ;
	BOOL		do_print ;
	BOOL		do_clear ;
	BOOL		do_record ;

	//----------------------------------------------
	ArmCpuSetup();
	cpu_ctrl_led(OFF);
	EMIFA_bus_cfg();
	HRK_port_init();
	init_uart();
	I2C_init(I2C0, I2C_CLK_100K);

	init_timer();
	init_RTC();
	init_printer();
	init_camera();
	//----------------------------------------------

	debug_off() ;

	init_screen() ;	
	init_OSD() ;
	init_motor() ;	
	init_device() ;
	init_LCD() ;
	MDIN240Init();

	load_ker_dpc_var() ;
	LCD_on() ;		

	init_font() ;	
	init_image() ;
	init_dictionary() ;
	init_display() ;
	
    init_system_settings(TRUE) ;

    IntMasterIRQEnable();
    IntGlobalEnable();
    IntIRQEnable();

 	start_system() ;

	load_LED_duty() ;
	init_MSR_mode() ;
	init_focus_state() ;
    init_comm_state() ;

	func_set	= FALSE ;
	do_save		= FALSE ;
	do_update	= TRUE ;
	do_print	= FALSE ;
	do_clear	= TRUE ;
    do_record	= FALSE ;

    // jig sw 20160223
	//start_jig_mode() ;

	while (1)
	{

		if (get_key_hit() != 0)
		{
			key_code	= get_key_code(FALSE) ;
			dsp_show_key_pressed(key_code) ;
			lcd_refresh() ;
			
			diffuser_stop() ;
			beep(20) ;

			switch (key_code)
			{
				case KEY_F1:
								if (is_key_pressed_for_longtime(KEY_F1) == TRUE)
								{
									hold_MSR_mode() ;
									if (run_user_setup() == TRUE) 
									{
										reset_MSR_mode() ;
										do_save		= TRUE ;
									}
									shift_MSR_mode(FALSE) ; 
								}
								else 
								{
									shift_MSR_mode(TRUE) ;
									func_set	= FALSE ;
								}
								break ;

				case KEY_F2:
								//	Return with clear. 
								//////////////////////////////////////////////////////////////////////////
								hold_MSR_mode() ; 
								if (run_result_display() == TRUE) {
									clear_MSR_mode(TRUE, FALSE) ;
								}
								shift_MSR_mode(FALSE) ; 
								break ;

				case KEY_F3:
								if (is_REF() == TRUE || is_KNR() == TRUE)
								{
									if (func_set == FALSE) {
										hold_MSR_mode() ; 
										run_acuity_map() ;
										shift_MSR_mode(FALSE) ;
									}
									else {
										shift_VD(FALSE) ;
									}
								}
								else if (is_KPH() == TRUE) 
								{
									move_KPH_next(FALSE) ;
								}
								else if (is_KER() == TRUE)
								{
									shift_KER_format(FALSE) ;
								}
								else if (is_CLBC() == TRUE)
								{
									clear_CLBC_data() ;
								}
								break ;

				case KEY_F4:
								if (is_REF() == TRUE || is_KER() == TRUE || is_KNR() == TRUE)
								{
									if (func_set == FALSE) { 
										shift_auto_start() ;
									}
									else { 
										hold_MSR_mode() ; 
										run_pupil_size() ;
										shift_MSR_mode(FALSE) ;
									}
								}
								else if (is_KPH() == TRUE)
								{
									if (func_set == FALSE) { 
										move_KPH_next(TRUE) ;
									}
									else { 
										hold_MSR_mode() ; 
										run_pupil_size() ;
										shift_MSR_mode(FALSE) ;
									}
								}
								else if (is_CLBC() == TRUE)
								{
									if (func_set == TRUE) {
										hold_MSR_mode() ; 
										run_pupil_size() ;
										shift_MSR_mode(FALSE) ;
									}
								}
								break ;

				case KEY_F5:
								if (is_REF() == TRUE || is_KNR() == TRUE)
								{
									if (func_set == FALSE) {
										toggle_IOL() ;
									}
									else {
										hold_MSR_mode() ; 
										run_illumination() ;
										shift_MSR_mode(FALSE) ;
									}
								}
								else if (func_set == TRUE) 
								{ 
									if (is_KER() == TRUE || is_KPH() == TRUE || is_CLBC() == TRUE) {
										hold_MSR_mode() ; 
										run_illumination() ;
										shift_MSR_mode(FALSE) ;
									}
								}
								else if (func_set == FALSE)
								{
									if (is_KPH() == TRUE) { 
										clear_KPH_data(FALSE) ;
									}
								}
								break ;

				case KEY_F6:
								func_set		= (func_set == TRUE ? FALSE : TRUE) ;
								break ;

				case KEY_PRT:
								//	send_LCD_screen_24bit() ;
								//break ;

								send_test_string();
								if (check_print_input(TRUE) == TRUE) {
									do_print	= TRUE ;
								}

								clear_key_input() ;
								break ;

				case KEY_F2_F5:
								break ;

				case KEY_F1_F4:
								break ;

				case KEY_F3_F6: 
								break ;

				default:
								break ;
			}

			insert_key_input(key_code) ;
			if (is_keyset_for_engineer_setup() == TRUE) {
				run_engineer_setup() ;
				shift_MSR_mode(FALSE) ;
			}

			wait_key_off() ;

			do_update		= TRUE ;
		}
		
		hlm_print() ;

		if (update_RL() == TRUE) 
		{
			do_update		= TRUE ;
			do_record		= FALSE ;
		}
		else if (comm_LM_data_recv() == RET_OK)
		{ 
			do_update		= TRUE ;
		}


		if (do_update == FALSE)
		{
			if (get_joystick_input() == BTN_PRESSED) { 
				if (is_auto_start()) { 
					reset_auto_count() ;
				}
			}

			if (get_joystick_input() == BTN_PRESSED || update_auto_start() == TRUE || check_longrun_test() == TRUE)
			{
				side	= get_RL() ;
				save_ref_led_duty() ;
				lcd_refresh() ;

				if (is_after_print() == TRUE)
				{
					set_after_print(FALSE) ;
					clear_MSR_data() ;
				}

				if (is_REF() == TRUE) 
				{
					if (run_REF_mode(side) == RET_ERR) { 
						diffuser_slow() ;
					}
				}
				else if (is_KER() == TRUE || is_CLBC() == TRUE) 
				{
					run_KER_mode(side) ;
				}
				else if (is_KPH() == TRUE) 
				{
					run_KPH_mode(side) ;
				}
				else if (is_KNR() == TRUE)
				{
					if (run_KNR_mode(side) == RET_ERR) { 
						diffuser_slow() ;
					}
				}

				restore_ref_led_duty() ;
				do_update		= TRUE ;	

				if (is_error() == FALSE) 
				{
					beep(5) ;
					
					if (is_KPH() == FALSE && is_CLBC() == FALSE) { 
						update_PD(FALSE) ;
					}
					if (is_auto_start() == TRUE) { 
						decrease_auto_count() ;
					}
					do_record		= TRUE ;
				}
				else 
				{
					beeps(2, 20, 50) ;
				}
			}
			else 
			{
				if (check_print_input(FALSE) == TRUE) {
					do_update	= TRUE ;
					do_print	= TRUE ;
				}

				if (is_after_measure()) { 
					if (is_REF() || is_KNR()) { 
						diffuser_slow() ;
					}
				}
			}
		}

		set_after_measure(do_record) ;
		
		if (do_update == TRUE)
		{
			if (do_print == TRUE)
			{
				run_print() ;
				run_data_transfer() ;

				if (is_after_print() == FALSE) { 
					increase_patient_count() ;
					set_after_print(TRUE) ;
				}

				clear_MSR_mode(FALSE, TRUE) ;
			}

			draw_measure_screen(func_set, do_clear) ;
			show_KPH_context(is_after_print()) ;
			reset_system() ;

			lcd_refresh() ;
			osd_refresh() ;
		
			if (do_save == TRUE)
			{
				show_osd_msg(OMSG_ON_SAVE_SETUP) ;
				sys_save_configure() ;
                init_comm_state();
				clear_osd_msg() ;
				beep(20) ;
			}
		}

		if (check_system() == FALSE) 
		{
			lcd_refresh() ;
			osd_refresh() ;

			do_update	= FALSE ;
			do_save		= FALSE ;
			do_print	= FALSE ;
			do_clear	= FALSE ;
			do_record	= FALSE ;
		}
		else 
		{
			do_update	= TRUE ;
		}
	}

	return ;

}



// jig sw 20160223
void start_jig_mode(void)
{
	int			ofs_x, ofs_y ;
	int			pre_x[2], pre_y[2] ;
	char		str_1[256] ;
	BOOL		jig_mode ;
	BOOL		new_mode ;
	int			kph_step ;
	int			key_code ;
	BOOL		do_update ;

	ofs_x		= 49 ;
	ofs_y		= 0 ;
	pre_x[0] = pre_x[1] = 0 ;
	pre_y[0] = pre_y[1] = 0 ;
	kph_step	= 0 ;

	lcd_clear_buff() ;
	mire_ring_off() ;
	focus_led_on() ;

	jig_mode	= TRUE ;
	new_mode	= FALSE ;
	do_update	= TRUE ;

	while (1)
	{
		if (get_key_hit() != 0)
		{
			key_code	= get_key_code(FALSE) ;
			dsp_show_key_pressed(key_code) ;
			lcd_refresh() ;

			switch (key_code)
			{
			case KEY_F1:
						if (jig_mode == TRUE) {
							mire_ring_on() ;
							focus_led_on() ;
							ext_led_on() ;
							jig_mode	= FALSE ;
							diffuser_stop() ;
						}
						else {
							mire_ring_off() ;
							focus_led_on() ;
							ext_led_off() ;
							jig_mode	= TRUE ;
							diffuser_start(TRUE) ;
						}
						break ;
			case KEY_F2:
						ofs_x	-= 1 ;
						ofs_x	= (ofs_x < 10 ? 10 : ofs_x) ;
						break ;
			case KEY_F3:
						ofs_x	+= 1 ;
						ofs_x	= (ofs_x > 120 ? 120 : ofs_x) ;
						break ;
			case KEY_F4:
						ofs_y	-= 1 ;
						ofs_y	= (ofs_y < -120 ? -120 : ofs_y) ;
						break ;
			case KEY_F5:
						ofs_y	+= 1 ;
						ofs_y	= (ofs_y > 120 ? 120 : ofs_y) ;
						break ;
			case KEY_F6:
						/*
						ofs_x		= 49 ;
						ofs_y		= 0 ;
						*/
						kph_step	+= 1 ;
						if (kph_step == 0) {
							kph_LED_control(KPH_LED_OFF) ;
						}
						else if (kph_step == 1) {
							kph_LED_control(KPH_LED_UP) ;
						}
						else if (kph_step == 2) {
							kph_LED_control(KPH_LED_DOWN) ;
						}
						else if (kph_step == 3) {
							kph_LED_control(KPH_LED_LEFT) ;
						}
						else if (kph_step == 4) {
							kph_LED_control(KPH_LED_RIGHT) ;
						}
						else {
							kph_step	= 0 ;
							kph_LED_control(KPH_LED_OFF) ;
						}
						break ;
			case KEY_PRT:
						if (new_mode == FALSE) {
							new_mode	= TRUE ;
							ofs_y		= 55 ;
						}
						else {
							new_mode	= FALSE ;
							ofs_y		= 0 ;
						}
						break ;

			}

			beep(50) ;
			do_update		= TRUE ;
		}

		if (do_update == TRUE)
		{
			osd_clear_buff() ;
			dsp_show_center_cross_line(TRUE) ;
			dsp_show_target_circle2(TRUE) ;
			osd_draw_rect(319-ofs_x-8, 239+ofs_y-8, 319-ofs_x+8, 240+ofs_y+8, 1, OSD_COLOR3, TRUE, FALSE) ;
			osd_draw_rect(320+ofs_x-8, 239+ofs_y-8, 320+ofs_x+8, 240+ofs_y+8, 1, OSD_COLOR3, TRUE, FALSE) ;
			dsp_show_menu_buttons(BTN_MODE, BTN_LEFT, BTN_RIGHT, BTN_UP, BTN_DOWN, BTN_KPH);
			do_update		= FALSE ;
		}

		if (jig_mode == TRUE)
		{
			//	Capture a frame.
			//////////////////////////////////////////////////////////////////////////
			scr_capture_frame(FALSE) ;

			//	Background Threshold.
			//////////////////////////////////////////////////////////////////////////
			ker_calc_background_threshold() ;

			if (ker_clip_focus_points() == RET_OK)
			{
				lcd_set_clear_mode(CLEAR_DEFAULT) ;
				lcd_set_font_set(LF_OUTLINE|LF_SMALL) ;
				lcd_set_char_color(RGB16(0xFF, 0x99, 0x00)) ;
				lcd_set_outl_color(RGB16(0x08, 0x08, 0x08)) ;

				sprintf(str_1, "%2d,%1d,%d", 319-((int) ker_calc.focus_cx[0]), 240-((int)ker_calc.focus_cy[0]), ker_calc.focus_size[0]) ;
				lcd_draw_text(str_1, 320-ofs_x-50, 240-ofs_y-50, 320-ofs_x+50, 240-ofs_y-20, LT_CENTER|LT_VCENTER) ;
				sprintf(str_1, "%2d,%1d,%d", ((int)ker_calc.focus_cx[1])-320, 240-((int)ker_calc.focus_cy[1]), ker_calc.focus_size[1]) ;
				lcd_draw_text(str_1, 320+ofs_x-50, 240-ofs_y-50, 320+ofs_x+50, 240-ofs_y-20, LT_CENTER|LT_VCENTER) ;
				sprintf(str_1, "FOCUS_LEN: %.2f", ker_calc.focus_len) ;
				lcd_draw_text(str_1, 50, 50, 600, 75, LT_LEFT|LT_VCENTER) ;
				sprintf(str_1, "XY OFFSET: %d, %d", ofs_x, ofs_y) ;
				lcd_draw_text(str_1, 50, 75, 400, 100, LT_LEFT|LT_VCENTER) ;
			}
		}

		lcd_refresh() ;
		osd_refresh() ;
	}

	return ;
}
