#ifndef _FONT__H__
#define _FONT__H__

#include "LiBOS_CORE.h"
#include "WINDOW.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32* get_A_data     (void);
UINT_32* get_B_data     (void);
UINT_32* get_C_data     (void);
UINT_32* get_D_data     (void);
UINT_32* get_E_data     (void);
UINT_32* get_F_data     (void);
UINT_32* get_G_data     (void);
UINT_32* get_H_data     (void);
UINT_32* get_I_data     (void);
UINT_32* get_J_data     (void);
UINT_32* get_K_data     (void);
UINT_32* get_L_data     (void);
UINT_32* get_M_data     (void);
UINT_32* get_N_data     (void);
UINT_32* get_O_data     (void);
UINT_32* get_P_data     (void);
UINT_32* get_Q_data     (void);
UINT_32* get_R_data     (void);
UINT_32* get_S_data     (void);
UINT_32* get_T_data     (void);
UINT_32* get_U_data     (void);
UINT_32* get_V_data     (void);
UINT_32* get_W_data     (void);
UINT_32* get_X_data     (void);
UINT_32* get_Y_data     (void);
UINT_32* get_Z_data     (void);
UINT_32* get_a_data     (void);
UINT_32* get_b_data     (void);
UINT_32* get_c_data     (void);
UINT_32* get_d_data     (void);
UINT_32* get_e_data     (void);
UINT_32* get_f_data     (void);
UINT_32* get_g_data     (void);
UINT_32* get_h_data     (void);
UINT_32* get_i_data     (void);
UINT_32* get_j_data     (void);
UINT_32* get_k_data     (void);
UINT_32* get_l_data     (void);
UINT_32* get_m_data     (void);
UINT_32* get_n_data     (void);
UINT_32* get_o_data     (void);
UINT_32* get_p_data     (void);
UINT_32* get_q_data     (void);
UINT_32* get_r_data     (void);
UINT_32* get_s_data     (void);
UINT_32* get_t_data     (void);
UINT_32* get_u_data     (void);
UINT_32* get_v_data     (void);
UINT_32* get_w_data     (void);
UINT_32* get_x_data     (void);
UINT_32* get_y_data     (void);
UINT_32* get_z_data     (void);
UINT_32* get_0_data     (void);
UINT_32* get_1_data     (void);
UINT_32* get_2_data     (void);
UINT_32* get_3_data     (void);
UINT_32* get_4_data     (void);
UINT_32* get_5_data     (void);
UINT_32* get_6_data     (void);
UINT_32* get_7_data     (void);
UINT_32* get_8_data     (void);
UINT_32* get_9_data     (void);
UINT_32* get_dot_data   (void);
UINT_32* get_comma_data (void);
UINT_32* get_coloum_data(void);
UINT_32* get_star_data  (void);
UINT_32* get_minus_data (void);
UINT_32* get_plus_data  (void);
UINT_32* get_equal_data (void);
UINT_32* get_danger_data(void);
UINT_32* get_space_data (void);
void     draw_string    (const INT_8* string, WINDOW* wnd, UINT_32 x, UINT_32 y, UINT_32 background_color);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif //! _FONT__H__
