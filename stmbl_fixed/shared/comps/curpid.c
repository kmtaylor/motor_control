#include "hal.h"
#include "defines.h"
#include "angle.h"

HAL_COMP(curpid);

// current command
HAL_PIN(id_cmd);
HAL_PIN(iq_cmd);

// current feedback
HAL_PIN(id_fb);
HAL_PIN(iq_fb);

// voltage output
HAL_PIN(ud);
HAL_PIN(uq);

// maximum output current and voltage
HAL_PIN(max_cur);
HAL_PIN(pwm_volt);

// d, q resistance and inductance
HAL_PIN(r);
HAL_PIN(l);

HAL_PIN(kp);
HAL_PIN(ki);

struct curpid_ctx_t {
  sat accum id_error_sum;
  sat accum iq_error_sum;
};

static void rt_func(accum period, volatile void *ctx_ptr, volatile hal_pin_inst_t *pin_ptr) {
  struct curpid_ctx_t *ctx      = (struct curpid_ctx_t *)ctx_ptr;
  struct curpid_pin_ctx_t *pins = (struct curpid_pin_ctx_t *)pin_ptr;

  accum r = PIN(r);
  accum l = PIN(l);

  accum kp  = l * PIN(kp) / period / 2.0K;
  accum ki  = r * PIN(ki) / l;

  accum max_cur = PIN(max_cur);
  accum idc     = PIN(id_cmd);
  accum iqc     = PIN(iq_cmd);

  idc = LIMIT(idc, max_cur);
  iqc = LIMIT(iqc, max_cur);

  sat accum max_volt = PIN(pwm_volt);

  accum id = PIN(id_fb);
  accum iq = PIN(iq_fb);

  sat accum id_error = idc - id;
  sat accum iq_error = iqc - iq;

  ctx->id_error_sum = ctx->id_error_sum + kp * ki * id_error;
  ctx->iq_error_sum = ctx->iq_error_sum + kp * ki * iq_error;

  sat accum ud = kp * id_error + ctx->id_error_sum;
  sat accum uq = kp * iq_error + ctx->iq_error_sum;

  uq = LIMIT(uq, max_volt); 
  ud = LIMIT(ud, max_volt); 

  PIN(ud) = ud;
  PIN(uq) = uq;
}

hal_comp_t curpid_comp_struct = {
    .name      = "curpid",
    .nrt       = 0,
    .rt        = rt_func,
    .frt       = 0,
    .nrt_init  = 0,
    .rt_start  = 0,
    .frt_start = 0,
    .rt_stop   = 0,
    .frt_stop  = 0,
    .ctx_size  = sizeof(struct curpid_ctx_t),
    .pin_count = sizeof(struct curpid_pin_ctx_t) / sizeof(struct hal_pin_inst_t),
};
