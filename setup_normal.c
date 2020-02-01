static void load_pos_pid(void) {
    load("encoder");
    load("step_dir");
    load("uvw");
    load("fb_switch");
    load("vel");
    load("vel");
    load("vel");
    load("pid");
    load("dbg");
}

static void init_pos_pid(void) {
    set_pin_val("encoder",      0, "rt_prio",  7);
    set_pin_val("step_dir",     0, "rt_prio",  7);
    set_pin_val("uvw",          0, "rt_prio",  8);
    set_pin_val("fb_switch",    0, "rt_prio",  9);
    set_pin_val("vel",          0, "rt_prio", 10);
    set_pin_val("vel",          1, "rt_prio", 10);
    set_pin_val("vel",          2, "rt_prio", 10);
    set_pin_val("pid",          0, "rt_prio", 12);
    set_pin_val("dbg",          0, "rt_prio", 14);

    set_pin_val("fb_switch", 0, "en",                1);
    set_pin_val("fb_switch", 0, "polecount",         2);
    set_pin_val("fb_switch", 0, "mot_polecount",     1);
    set_pin_val("fb_switch", 0, "com_polecount",     2);
    set_pin_val("fb_switch", 0, "mot_offset", M_PI/6.0);
    set_pin_val("fb_switch", 0, "com_offset", M_PI/6.0);
    set_pin_val("fb_switch", 0, "com_state",         3);

    set_pin_val("vel", 0, "en", 1);
    set_pin_val("vel", 1, "en", 1);
    set_pin_val("vel", 2, "en", 1);
    set_pin_val("vel", 0, "w",  200);
    set_pin_val("vel", 1, "w",  200);
    set_pin_val("vel", 2, "w",  200);

    set_pin_val("pid", 0, "enable",             1);
    set_pin_val("pid", 0, "pos_p",             10);
    set_pin_val("pid", 0, "vel_p",            100);
    set_pin_val("pid", 0, "vel_i",              0);
    set_pin_val("pid", 0, "cur_gain",      420e-6); // From sm060
    set_pin_val("pid", 0, "max_usr_vel",      800); // Default
    set_pin_val("pid", 0, "max_usr_acc",    80000); // Default
    set_pin_val("pid", 0, "max_usr_torque", 80000); // Default

    connect_pins("uvw", 0, "u", "encoder", 0, "u");
    connect_pins("uvw", 0, "v", "encoder", 0, "v");
    connect_pins("uvw", 0, "w", "encoder", 0, "w");

    /* Commutation feedback switch (changes on detection of encoder index) */
    connect_pins("fb_switch", 0, "com_abs_pos", "uvw",      0, "pos");
    connect_pins("fb_switch", 0, "mot_abs_pos", "encoder",  0, "mot_abs_pos");
    connect_pins("fb_switch", 0, "mot_state",   "encoder",  0, "mot_state");

    /* Commutation motor model - generates position for dq/idq */
    connect_pins("vel", 2, "pos_in", "fb_switch",   0, "com_fb"); 
    connect_pins("vel", 2, "torque", "pid",         0, "torque_cor_cmd");

    /* Position motor model, will jump on first index, but should be OK */
    connect_pins("vel", 1, "pos_in", "encoder", 0, "mot_abs_pos");
    connect_pins("vel", 1, "torque", "pid",     0, "torque_cor_cmd");

    /* External command */
    connect_pins("vel", 0, "pos_in", "step_dir", 0, "pos");

    /* Position PID */
    connect_pins("pid", 0, "pos_ext_cmd",   "step_dir", 0, "pos");
    connect_pins("pid", 0, "vel_ext_cmd",   "vel",      0, "vel");
    connect_pins("pid", 0, "pos_fb",        "encoder",  0, "mot_abs_pos");
    connect_pins("pid", 0, "vel_fb",        "vel",      1, "vel");

    /* Drive curpid loop */
    connect_pins("curpid", 0, "iq_cmd", "pid",  0, "cur_cor_cmd");
    connect_pins("dq",     0, "pos",    "vel",  2, "pos_out");
    connect_pins("idq",    0, "pos",    "vel",  2, "pos_out");

    /* Debug */
    connect_pins("dbg", 0, "in0", "step_dir",   0, "pos");
    connect_pins("dbg", 0, "in1", "vel",        0, "vel");
    connect_pins("dbg", 0, "in2", "encoder",    0, "mot_abs_pos");
    connect_pins("dbg", 0, "in3", "vel",        1, "vel");
    connect_pins("dbg", 0, "in4", "pid",        0, "torque_cor_cmd");
}
