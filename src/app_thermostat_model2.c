#include "app_main.h"

/* data point for manufacturer id -
 * "edl8pz1k"
 *
 * id, type, len, divisor, remote_commands_functionCb, local_commands_functionCb
 */
data_point_st_t data_point_model2[DP_IDX_MAXNUM] = {
        {DP_TYPE2_ID_01, DP_BOOL, 1,    1,  remote_cmd_sys_mode_2, local_cmd_onoff_state_2},                // onoff
        {DP_TYPE2_ID_18, DP_VAL,  4,    10, NULL, local_cmd_inner_sensor_2},                                // local temperature
        {DP_TYPE2_ID_10, DP_VAL,  4,    10, remote_cmd_heating_set_2, local_cmd_heating_set_2},             // heat setpoint
        {DP_TYPE2_ID_1A, DP_VAL,  4,    10, remote_cmd_min_setpoint_2, local_cmd_min_setpoint_2},           // min heat setpoint
        {DP_TYPE2_ID_13, DP_VAL,  4,    10, remote_cmd_max_setpoint_2, local_cmd_max_setpoint_2},           // max heat setpoint
        {DP_TYPE2_ID_67, DP_VAL,  4,    1,  remote_cmd_deadband_2, local_cmd_deadband_2},                   // hysteresis
        {DP_TYPE2_ID_1B, DP_VAL,  4,    1,  remote_cmd_temp_calibration_2, local_cmd_temp_calibration_2},   // local temperature calibration
        {DP_TYPE2_ID_24, DP_ENUM, 1,    1,  NULL, local_cmd_set_run_state_2},                       // 0x00 - heat, 0x01 - idle
        {DP_TYPE2_ID_2B, DP_ENUM, 1,    1,  remote_cmd_sensor_used_2, local_cmd_sensor_used_2},     // sensor IN/AL/OU
        {DP_TYPE2_ID_02, DP_ENUM, 1,    1,  remote_cmd_oper_mode_2, local_cmd_oper_mode_2},         // manual (setpoint) / programming (schedule)
        {DP_TYPE2_ID_28, DP_BOOL, 1,    1,  remote_cmd_keylock_2, local_cmd_keylock_2},             // lock / unlock keys (child lock)
        {DP_TYPE2_ID_00, DP_RAW,  0,    1,  remote_cmd_set_schedule_2, NULL},                       // schedule
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            // unknown
        {DP_TYPE2_ID_66, DP_VAL,  4,    10, NULL, local_cmd_outdoor_sensor_2},                      // temperature of outer sensor
        {DP_TYPE2_ID_6F, DP_VAL,  4,    10, remote_cmd_frost_protect_2, local_cmd_frost_protect_2}, // frost protected
        {DP_TYPE2_ID_68, DP_VAL,  4,    10, remote_cmd_heat_protect_2, local_cmd_heat_protect_2},   // heat protected
        {DP_TYPE2_ID_6E, DP_RAW,  0x11, 10, NULL, local_cmd_set_schedule_2},                        // schedule mon
        {DP_TYPE2_ID_6D, DP_RAW,  0x11, 10, NULL, local_cmd_set_schedule_2},                        // schedule tue
        {DP_TYPE2_ID_6C, DP_RAW,  0x11, 10, NULL, local_cmd_set_schedule_2},                        // schedule wed
        {DP_TYPE2_ID_6B, DP_RAW,  0x11, 10, NULL, local_cmd_set_schedule_2},                        // schedule thu
        {DP_TYPE2_ID_6A, DP_RAW,  0x11, 10, NULL, local_cmd_set_schedule_2},                        // schedule fri
        {DP_TYPE2_ID_69, DP_RAW,  0x11, 10, NULL, local_cmd_set_schedule_2},                        // schedule sat
        {DP_TYPE2_ID_65, DP_RAW,  0x11, 10, NULL, local_cmd_set_schedule_2},                        // schedule sun
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
        {DP_TYPE2_ID_00, DP_RAW,  0,    0,  NULL, NULL},                                            //
};

/*
 *
 * Local variables and functions
 *
 */

static void get_schedule(void *args) {

    if(!zb_isDeviceJoinedNwk()) return;

    uint8_t *day = (uint8_t*)args;

    epInfo_t dstEpInfo;
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);

    dstEpInfo.profileId = HA_PROFILE_ID;
#if FIND_AND_BIND_SUPPORT
    dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
#else
    dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
    dstEpInfo.dstEp = APP_ENDPOINT1;
    dstEpInfo.dstAddr.shortAddr = 0x0;
#endif

    zcl_thermostat_setWeeklyScheduleCmd_t cmd;
    heatMode_t *heat_mode;

    cmd.dayOfWeekForSequence = 0;

        if (*day & DAY_MON) {
            //mon
            heat_mode =  g_zcl_scheduleData.schedule_mon;
            cmd.dayOfWeekForSequence = DAY_MON;
        } else if (*day & DAY_TUE) {
            //tue
            heat_mode =  g_zcl_scheduleData.schedule_tue;
            cmd.dayOfWeekForSequence = DAY_TUE;
        } else if (*day & DAY_WED) {
            //wed
            heat_mode =  g_zcl_scheduleData.schedule_wed;
            cmd.dayOfWeekForSequence = DAY_WED;
        } else if (*day & DAY_THU) {
            //thu
            heat_mode =  g_zcl_scheduleData.schedule_thu;
            cmd.dayOfWeekForSequence = DAY_THU;
        } else if (*day & DAY_FRI) {
            //fri
            heat_mode =  g_zcl_scheduleData.schedule_fri;
            cmd.dayOfWeekForSequence = DAY_FRI;
        } else if (*day & DAY_SAT) {
            //sat
            heat_mode =  g_zcl_scheduleData.schedule_sat;
            cmd.dayOfWeekForSequence = DAY_SAT;
        } else if (*day & DAY_SUN) {
            //sun
            heat_mode =  g_zcl_scheduleData.schedule_sun;
            cmd.dayOfWeekForSequence = DAY_SUN;
        } else {
            return;
        }

        cmd.numOfTransForSequence = 4;
        cmd.sequenceMode.pHeatMode = heat_mode;
        cmd.modeForSequence = HEAT_SERPOINT_FIELD_PRESENT;

//        for (uint8_t i = 0; i < cmd.numOfTransForSequence; i++) {
//            printf("i: %d, time: %d, temp: %d\r\n", i, cmd.sequenceMode.pHeatMode[i].transTime, cmd.sequenceMode.pHeatMode[i].heatSetpoint);
//        }

    zcl_thermostat_setWeeklyScheduleCmdSend(APP_ENDPOINT1, &dstEpInfo, 0, &cmd);
}


/*
 *
 * For models   "edl8pz1k"
 *
 * Funstions for local commands. MCU -> ZT3L -> Zegbee
 */

void local_cmd_oper_mode_2(void *args) {

    uint8_t *mode = (uint8_t*)args;

    if (*mode) {
        *mode = 0;
    } else {
        *mode = 1;
    }
#if UART_PRINTF_MODE && DEBUG_CMD
            printf("Thermostat in %s mode\r\n", *mode?"manual":"programming");
#endif



    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_HAVC_THERMOSTAT,
                   ZCL_ATTRID_HVAC_THERMOSTAT_PROGRAMMING_OPERATION_MODE, mode);

    thermostat_settings_save();
}

void local_cmd_frost_protect_2(void *args) {

    int16_t *temp = (int16_t*)args;
    uint16_t divisor = 1;
    int16_t  old_temp;
    uint16_t len;

    if (data_point_model[DP_IDX_FROST_PROTECT].divisor == 1) {
        divisor = 100;
    } else if (data_point_model[DP_IDX_FROST_PROTECT].divisor == 10) {
        divisor = 10;
    }

    *temp *= divisor;

    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_HAVC_THERMOSTAT,
                   ZCL_ATTRID_HVAC_THERMOSTAT_CUSTOM_FROST_PROTECT, &len, (uint8_t*)&old_temp);

    if (old_temp != *temp) {

        zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_HAVC_THERMOSTAT,
                       ZCL_ATTRID_HVAC_THERMOSTAT_CUSTOM_FROST_PROTECT, (uint8_t*)temp);

        thermostat_settings_save();
    }
}

void local_cmd_heat_protect_2(void *args) {

    int16_t *temp = (int16_t*)args;
    uint16_t divisor = 1;
    int16_t  old_temp;
    uint16_t len;

    if (data_point_model[DP_IDX_HEAT_PROTECT].divisor == 1) {
        divisor = 100;
    } else if (data_point_model[DP_IDX_HEAT_PROTECT].divisor == 10) {
        divisor = 10;
    }

    *temp *= divisor;

    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_HAVC_THERMOSTAT,
                   ZCL_ATTRID_HVAC_THERMOSTAT_CUSTOM_HEAT_PROTECT, &len, (uint8_t*)&old_temp);

    if (old_temp != *temp) {

        zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_HAVC_THERMOSTAT,
                       ZCL_ATTRID_HVAC_THERMOSTAT_CUSTOM_HEAT_PROTECT, (uint8_t*)temp);

        thermostat_settings_save();
    }
}

void local_cmd_outdoor_sensor_2(void *args) {

    int16_t *temp = (int16_t*)args;
    uint16_t divisor = 1;

    if (data_point_model[DP_IDX_TEMP_OUT].divisor == 1) {
        divisor = 100;
    } else if (data_point_model[DP_IDX_TEMP_OUT].divisor == 10) {
        divisor = 10;
    }

    *temp *= divisor;

    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_HAVC_THERMOSTAT,
                   ZCL_ATTRID_HVAC_THERMOSTAT_OUTDOOR_TEMPERATURE, (uint8_t*)temp);

#if UART_PRINTF_MODE && DEBUG_CMD
            printf("OutDoor Temperature: %d\r\n", *temp);
#endif

}

void local_cmd_set_schedule_2(void *args) {

    schedule_args_model2_t *schedule_args = (schedule_args_model2_t*)args;
    uint8_t *ptr = schedule_args->data_point->data;
    ptr++;
    dp_schedule_model2_t *schedule = (dp_schedule_model2_t*)ptr;

    for (uint8_t i = 0; i < 4; i++) {
        schedule_args->heatMode[i].transTime = schedule->hour * 60;
        schedule_args->heatMode[i].transTime += schedule->minute;
        schedule_args->heatMode[i].heatSetpoint = reverse16(schedule->temperature)/data_point_model[schedule_args->idx].divisor * 100;
        schedule++;
//        printf("idx: %d, i: %d, time: %d, temp: %d\r\n", schedule_args->idx, i, schedule_args->heatMode[i].transTime, schedule_args->heatMode[i].heatSetpoint);
    }

    thermostat_settings_save();

    w_day = schedule_args->w_day;

    TL_SCHEDULE_TASK(get_schedule, &w_day);
}

/*
 *
 * Functions for remote commands. Zegbee -> ZT3L -> MCU
 *
 */

void remote_cmd_oper_mode_2(void *args) {

    uint8_t oper_mode;
    uint8_t *arg = (uint8_t*)args;

    if (*arg) {
        oper_mode = 0;
    } else {
        oper_mode = 1;
    }

    if (data_point_model[DP_IDX_PROG].id == 0) return;

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;
    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(5);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_PROG].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_PROG].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(1));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;
    data_point->data[0] = oper_mode;
    out_pkt->pkt_len ++;
    data_point->data[1] = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    thermostat_settings_save();
}

void remote_cmd_frost_protect_2(void *args) {

    int16_t *arg = (int16_t*)args;
    uint32_t frost_protect = *arg;

    if (data_point_model[DP_IDX_FROST_PROTECT].id == 0) return;

    if (frost_protect < FROST_PROTECT_MIN * 100 || frost_protect > FROST_PROTECT_MAX * 100) {
        return;
    }

    frost_protect /= 100; // 1500 -> 15°C

    if (data_point_model[DP_IDX_FROST_PROTECT].divisor == 10) {
        frost_protect *= 10;
    } else if (data_point_model[DP_IDX_FROST_PROTECT].divisor == 100) {
        frost_protect *= 100;
    }

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;
    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(8);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_FROST_PROTECT].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_FROST_PROTECT].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(4));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;
    data_point->data[0] = (frost_protect >> 24) & 0xFF;
    data_point->data[1] = (frost_protect >> 16) & 0xFF;
    data_point->data[2] = (frost_protect >> 8)  & 0xFF;
    data_point->data[3] = frost_protect & 0xFF;
    out_pkt->pkt_len += 4;
    data_point->data[4] = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    thermostat_settings_save();

}

void remote_cmd_heat_protect_2(void *args) {

    int16_t *arg = (int16_t*)args;
    uint32_t heat_protect = *arg;

    if (data_point_model[DP_IDX_HEAT_PROTECT].id == 0) return;

    if (heat_protect < HEAT_PROTECT_MIN * 100 || heat_protect > HEAT_PROTECT_MAX * 100) {
        return;
    }

    heat_protect /= 100; // 1500 -> 15°C

    if (data_point_model[DP_IDX_HEAT_PROTECT].divisor == 10) {
        heat_protect *= 10;
    } else if (data_point_model[DP_IDX_HEAT_PROTECT].divisor == 100) {
        heat_protect *= 100;
    }

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;
    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(8);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_HEAT_PROTECT].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_HEAT_PROTECT].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(4));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;
    data_point->data[0] = (heat_protect >> 24) & 0xFF;
    data_point->data[1] = (heat_protect >> 16) & 0xFF;
    data_point->data[2] = (heat_protect >> 8)  & 0xFF;
    data_point->data[3] = heat_protect & 0xFF;
    out_pkt->pkt_len += 4;
    data_point->data[4] = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    thermostat_settings_save();

}

static void remote_cmd_set_schedule_mon() {

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;

    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(21);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_SCHEDULE_MON].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_SCHEDULE_MON].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(data_point_model[DP_IDX_SCHEDULE_MON].len));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    uint8_t *pstrd = data_point->data;
    heatMode_t *heat_mode = g_zcl_scheduleData.schedule_mon;
    uint16_t temp;

    *pstrd++ = 1;
    out_pkt->pkt_len++;

    for (uint8_t i = 0; i < 4; i++) {
        *pstrd++ = heat_mode[i].transTime / 60;
        out_pkt->pkt_len++;
        *pstrd++ = heat_mode[i].transTime % 60;
        out_pkt->pkt_len++;
        temp = (heat_mode[i].heatSetpoint / 100) * data_point_model[DP_IDX_SCHEDULE_MON].divisor;
        *pstrd++ = (temp >> 8)  & 0xFF;
        *pstrd++ = temp & 0xFF;
        out_pkt->pkt_len += 2;
    }

    *pstrd = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    TL_SCHEDULE_TASK(get_schedule, &w_mon);
}

static void remote_cmd_set_schedule_tue() {

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;

    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(21);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_SCHEDULE_TUE].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_SCHEDULE_TUE].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(data_point_model[DP_IDX_SCHEDULE_TUE].len));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    uint8_t *pstrd = data_point->data;
    heatMode_t *heat_mode = g_zcl_scheduleData.schedule_tue;
    uint16_t temp;

    *pstrd++ = 1;
    out_pkt->pkt_len++;

    for (uint8_t i = 0; i < 4; i++) {
        *pstrd++ = heat_mode[i].transTime / 60;
        out_pkt->pkt_len++;
        *pstrd++ = heat_mode[i].transTime % 60;
        out_pkt->pkt_len++;
        temp = (heat_mode[i].heatSetpoint / 100) * data_point_model[DP_IDX_SCHEDULE_TUE].divisor;
        *pstrd++ = (temp >> 8)  & 0xFF;
        *pstrd++ = temp & 0xFF;
        out_pkt->pkt_len += 2;
    }

    *pstrd = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    TL_SCHEDULE_TASK(get_schedule, &w_tue);
}

static void remote_cmd_set_schedule_wed() {

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;

    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(21);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_SCHEDULE_WED].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_SCHEDULE_WED].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(data_point_model[DP_IDX_SCHEDULE_WED].len));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    uint8_t *pstrd = data_point->data;
    heatMode_t *heat_mode = g_zcl_scheduleData.schedule_wed;
    uint16_t temp;

    *pstrd++ = 1;
    out_pkt->pkt_len++;

    for (uint8_t i = 0; i < 4; i++) {
        *pstrd++ = heat_mode[i].transTime / 60;
        out_pkt->pkt_len++;
        *pstrd++ = heat_mode[i].transTime % 60;
        out_pkt->pkt_len++;
        temp = (heat_mode[i].heatSetpoint / 100) * data_point_model[DP_IDX_SCHEDULE_WED].divisor;
        *pstrd++ = (temp >> 8)  & 0xFF;
        *pstrd++ = temp & 0xFF;
        out_pkt->pkt_len += 2;
    }

    *pstrd = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    TL_SCHEDULE_TASK(get_schedule, &w_wed);
}

static void remote_cmd_set_schedule_thu() {

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;

    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(21);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_SCHEDULE_THU].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_SCHEDULE_THU].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(data_point_model[DP_IDX_SCHEDULE_THU].len));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    uint8_t *pstrd = data_point->data;
    heatMode_t *heat_mode = g_zcl_scheduleData.schedule_thu;
    uint16_t temp;

    *pstrd++ = 1;
    out_pkt->pkt_len++;

    for (uint8_t i = 0; i < 4; i++) {
        *pstrd++ = heat_mode[i].transTime / 60;
        out_pkt->pkt_len++;
        *pstrd++ = heat_mode[i].transTime % 60;
        out_pkt->pkt_len++;
        temp = (heat_mode[i].heatSetpoint / 100) * data_point_model[DP_IDX_SCHEDULE_THU].divisor;
        *pstrd++ = (temp >> 8)  & 0xFF;
        *pstrd++ = temp & 0xFF;
        out_pkt->pkt_len += 2;
    }

    *pstrd = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    TL_SCHEDULE_TASK(get_schedule, &w_thu);
}

static void remote_cmd_set_schedule_fri() {

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;

    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(21);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_SCHEDULE_FRI].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_SCHEDULE_FRI].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(data_point_model[DP_IDX_SCHEDULE_FRI].len));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    uint8_t *pstrd = data_point->data;
    heatMode_t *heat_mode = g_zcl_scheduleData.schedule_fri;
    uint16_t temp;

    *pstrd++ = 1;
    out_pkt->pkt_len++;

    for (uint8_t i = 0; i < 4; i++) {
        *pstrd++ = heat_mode[i].transTime / 60;
        out_pkt->pkt_len++;
        *pstrd++ = heat_mode[i].transTime % 60;
        out_pkt->pkt_len++;
        temp = (heat_mode[i].heatSetpoint / 100) * data_point_model[DP_IDX_SCHEDULE_FRI].divisor;
        *pstrd++ = (temp >> 8)  & 0xFF;
        *pstrd++ = temp & 0xFF;
        out_pkt->pkt_len += 2;
    }

    *pstrd = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    TL_SCHEDULE_TASK(get_schedule, &w_fri);
}

static void remote_cmd_set_schedule_sat() {

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;

    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(21);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_SCHEDULE_SAT].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_SCHEDULE_SAT].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(data_point_model[DP_IDX_SCHEDULE_SAT].len));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    uint8_t *pstrd = data_point->data;
    heatMode_t *heat_mode = g_zcl_scheduleData.schedule_sat;
    uint16_t temp;

    *pstrd++ = 1;
    out_pkt->pkt_len++;

    for (uint8_t i = 0; i < 4; i++) {
        *pstrd++ = heat_mode[i].transTime / 60;
        out_pkt->pkt_len++;
        *pstrd++ = heat_mode[i].transTime % 60;
        out_pkt->pkt_len++;
        temp = (heat_mode[i].heatSetpoint / 100) * data_point_model[DP_IDX_SCHEDULE_SAT].divisor;
        *pstrd++ = (temp >> 8)  & 0xFF;
        *pstrd++ = temp & 0xFF;
        out_pkt->pkt_len += 2;
    }

    *pstrd = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    TL_SCHEDULE_TASK(get_schedule, &w_sat);
}

static void remote_cmd_set_schedule_sun() {

    pkt_tuya_t *out_pkt = (pkt_tuya_t*)remote_cmd_pkt_buff;

    uint16_t seq_num = get_seq_num();
    seq_num++;

    set_header_pkt(remote_cmd_pkt_buff, sizeof(remote_cmd_pkt_buff), seq_num, COMMAND04);

    out_pkt->len = reverse16(21);
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    data_point_t *data_point = (data_point_t*)out_pkt->data;
    data_point->dp_id = data_point_model[DP_IDX_SCHEDULE_SUN].id;
    out_pkt->pkt_len++;
    data_point->dp_type = data_point_model[DP_IDX_SCHEDULE_SUN].type;
    out_pkt->pkt_len++;
    data_point->dp_len = (reverse16(data_point_model[DP_IDX_SCHEDULE_SUN].len));
    out_pkt->pkt_len++;
    out_pkt->pkt_len++;

    uint8_t *pstrd = data_point->data;
    heatMode_t *heat_mode = g_zcl_scheduleData.schedule_sun;
    uint16_t temp;

    *pstrd++ = 7;
    out_pkt->pkt_len++;

    for (uint8_t i = 0; i < 4; i++) {
        *pstrd++ = heat_mode[i].transTime / 60;
        out_pkt->pkt_len++;
        *pstrd++ = heat_mode[i].transTime % 60;
        out_pkt->pkt_len++;
        temp = (heat_mode[i].heatSetpoint / 100) * data_point_model[DP_IDX_SCHEDULE_SUN].divisor;
        *pstrd++ = (temp >> 8)  & 0xFF;
        *pstrd++ = temp & 0xFF;
        out_pkt->pkt_len += 2;
    }

    *pstrd = checksum((uint8_t*)out_pkt, out_pkt->pkt_len++);
    add_to_ring_cmd(out_pkt, true);

    set_seq_num(seq_num);

    TL_SCHEDULE_TASK(get_schedule, &w_sun);
}

void remote_cmd_set_schedule_2(void *args) {

//    if (data_point_model[DP_IDX_SCHEDULE].remote_cmd == NULL) return;

    uint8_t *dayOfWeek = (uint8_t*)args;

    if (*dayOfWeek & DAY_SUN) {
        remote_cmd_set_schedule_sun();
    }

    if (*dayOfWeek & DAY_MON) {
        remote_cmd_set_schedule_mon();
    }

    if (*dayOfWeek & DAY_TUE) {
        remote_cmd_set_schedule_tue();
    }

    if (*dayOfWeek & DAY_WED) {
        remote_cmd_set_schedule_wed();
    }

    if (*dayOfWeek & DAY_THU) {
        remote_cmd_set_schedule_thu();
    }

    if (*dayOfWeek & DAY_FRI) {
        remote_cmd_set_schedule_fri();
    }

    if (*dayOfWeek & DAY_SAT) {
        remote_cmd_set_schedule_sat();
    }

    thermostat_settings_save();
}

void remote_cmd_get_schedule_2() {

    TL_SCHEDULE_TASK(get_schedule, &w_mon);
    TL_SCHEDULE_TASK(get_schedule, &w_tue);
    TL_SCHEDULE_TASK(get_schedule, &w_wed);
    TL_SCHEDULE_TASK(get_schedule, &w_thu);
    TL_SCHEDULE_TASK(get_schedule, &w_fri);
    TL_SCHEDULE_TASK(get_schedule, &w_sat);
    TL_SCHEDULE_TASK(get_schedule, &w_sun);
}

