#include <project.h>
#include "hal.h"
#include "defines.h"
#include "angle.h"
#include "constants.h"

HAL_COMP(encoder);

HAL_PIN(com_pos);
HAL_PIN(mot_pos);

struct encoder_ctx_t {
    int index_offset;
    int index_found;
};

static void nrt_init(volatile void *ctx_ptr, volatile hal_pin_inst_t *pin_ptr) {
    //struct encoder_ctx_t *ctx = (struct encoder_ctx_t *) ctx_ptr;
    //struct encoder_pin_ctx_t *pins = (struct encoder_pin_ctx_t *) pin_ptr;

    QuadDec_0_Start();
    //QuadDec_2_Start();
    QuadDec_0_WriteCounter(0x8000);
    //QuadDec_2_WriteCounter(0x8000);
}

static void rt_func(accum period, volatile void *ctx_ptr, volatile hal_pin_inst_t *pin_ptr) {
    struct encoder_ctx_t *ctx      = (struct encoder_ctx_t *) ctx_ptr;
    struct encoder_pin_ctx_t *pins = (struct encoder_pin_ctx_t *) pin_ptr;
    int count, idx;
    accum com_abs_pos, mot_abs_pos;
    const accum t[8] = {
        0, 0, 2K*M_PI/3K,  M_PI/3K, 
             -2K*M_PI/3K, -M_PI/3K, -M_PI, 0};
  
    idx = (QUAD1_U_Read() << 2) | (QUAD1_V_Read() << 1) | (QUAD1_W_Read() << 0);

    com_abs_pos = t[idx];

    if (!ctx->index_found &&
            QuadDec_0_ReadStatusRegister() & QuadDec_0_STATUS_CAPTURE) {
        count = QuadDec_0_ReadCapture() - 0x8000;
        count = (count <= 0) ? -count : 2000 - count;
        ctx->index_offset = count;
        ctx->index_found = 1;
    }

    count = QuadDec_0_ReadCounter() - 0x8000;
    count = (count <= 0) ? -count : 2000 - count;

    mot_abs_pos = mod((accum) count * (M_PI / 1000K));

    if (!ctx->index_found) {
        PIN(com_pos) = mod((com_abs_pos + FB_COM_OFFSET) * (FB_POLECOUNT / FB_COM_POLECOUNT));
    } else {
        PIN(com_pos) = mod((mot_abs_pos + FB_MOT_OFFSET) * (FB_POLECOUNT / FB_MOT_POLECOUNT));
    }

    count += ctx->index_offset;
    PIN(mot_pos) = mod((accum) count * (M_PI / 1000K));
}

hal_comp_t encoder_comp_struct = {
    .name      = "encoder",
    .nrt       = 0,
    .rt        = rt_func,
    .frt       = 0,
    .nrt_init  = nrt_init,
    .rt_start  = 0,
    .frt_start = 0,
    .rt_stop   = 0,
    .frt_stop  = 0,
    .ctx_size  = sizeof(struct encoder_ctx_t),
    .pin_count = sizeof(struct encoder_pin_ctx_t) / sizeof(struct hal_pin_inst_t),
};
