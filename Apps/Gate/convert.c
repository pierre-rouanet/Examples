#include "convert.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gate.h"
#include "json_alloc.h"
#include "luos_utils.h"

// Create msg from a container json data
void json_to_msg(container_t *container, uint16_t id, luos_type_t type, cJSON *jobj, msg_t *msg, char *bin_data)
{
    time_luos_t time;
    float data = 0.0;
    cJSON *item;
    msg->header.target_mode = IDACK;
    msg->header.target = id;
    //********** global convertion***********
    // ratio
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "power_ratio")))
    {
        ratio_t ratio = (ratio_t)cJSON_GetObjectItem(jobj, "power_ratio")->valuedouble;
        RatioOD_RatioToMsg(&ratio, msg);
        Luos_SendMsg(container, msg);
    }
    // target angular position
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "target_rot_position")))
    {
        angular_position_t angular_position = (angular_position_t)cJSON_GetObjectItem(jobj, "target_rot_position")->valuedouble;
        AngularOD_PositionToMsg(&angular_position, msg);
        Luos_SendMsg(container, msg);
    }
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "target_rot_position")))
    {
        int i = 0;
        // this is a trajectory
        item = cJSON_GetObjectItem(jobj, "target_rot_position");
        int size = (int)cJSON_GetArrayItem(item, 0)->valueint;
        // find the first \r of the current buf
        for (i = 0; i < JSON_BUFF_SIZE; i++)
        {
            if (bin_data[i] == '\r')
            {
                i++;
                break;
            }
        }
        if (i < JSON_BUFF_SIZE - 1)
        {
            msg->header.cmd = ANGULAR_POSITION;
            Luos_SendData(container, msg, &bin_data[i], (unsigned int)size);
        }
    }
    // Limit angular position
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "limit_rot_position")))
    {
        angular_position_t limits[2];
        item = cJSON_GetObjectItem(jobj, "limit_rot_position");
        limits[0] = AngularOD_PositionFrom_deg(cJSON_GetArrayItem(item, 0)->valuedouble);
        limits[1] = AngularOD_PositionFrom_deg(cJSON_GetArrayItem(item, 1)->valuedouble);
        memcpy(&msg->data[0], limits, 2 * sizeof(float));
        msg->header.cmd = ANGULAR_POSITION_LIMIT;
        msg->header.size = 2 * sizeof(float);
        Luos_SendMsg(container, msg);
    }
    // Limit linear position
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "limit_trans_position")))
    {
        linear_position_t limits[2];
        item = cJSON_GetObjectItem(jobj, "limit_trans_position");
        limits[0] = LinearOD_PositionFrom_mm((float)cJSON_GetArrayItem(item, 0)->valuedouble);
        limits[1] = LinearOD_PositionFrom_mm((float)cJSON_GetArrayItem(item, 1)->valuedouble);
        memcpy(&msg->data[0], limits, 2 * sizeof(linear_position_t));
        msg->header.cmd = LINEAR_POSITION_LIMIT;
        msg->header.size = 2 * sizeof(linear_position_t);
        Luos_SendMsg(container, msg);
    }
    // Limit angular speed
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "limit_rot_speed")))
    {
        angular_speed_t limits[2];
        item = cJSON_GetObjectItem(jobj, "limit_rot_speed");
        limits[0] = AngularOD_SpeedFrom_deg_s(cJSON_GetArrayItem(item, 0)->valuedouble);
        limits[1] = AngularOD_SpeedFrom_deg_s(cJSON_GetArrayItem(item, 1)->valuedouble);
        memcpy(&msg->data[0], limits, 2 * sizeof(float));
        msg->header.cmd = ANGULAR_SPEED_LIMIT;
        msg->header.size = 2 * sizeof(float);
        Luos_SendMsg(container, msg);
    }
    // Limit linear speed
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "limit_trans_speed")))
    {
        linear_speed_t limits[2];
        item = cJSON_GetObjectItem(jobj, "limit_trans_speed");
        limits[0] = LinearOD_Speedfrom_mm_s((float)cJSON_GetArrayItem(item, 0)->valuedouble);
        limits[1] = LinearOD_Speedfrom_mm_s((float)cJSON_GetArrayItem(item, 1)->valuedouble);
        memcpy(&msg->data[0], limits, 2 * sizeof(linear_speed_t));
        msg->header.cmd = LINEAR_SPEED_LIMIT;
        msg->header.size = 2 * sizeof(linear_speed_t);
        Luos_SendMsg(container, msg);
    }
    // Limit ratio
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "limit_power")))
    {
        data = (float)cJSON_GetObjectItem(jobj, "limit_power")->valuedouble;
        memcpy(msg->data, &data, sizeof(data));
        msg->header.cmd = RATIO_LIMIT;
        msg->header.size = sizeof(float);
        Luos_SendMsg(container, msg);
    }
    // Limit current
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "limit_current")))
    {
        current_t current = (current_t)cJSON_GetObjectItem(jobj, "limit_current")->valuedouble;
        ElectricOD_CurrentToMsg(&current, msg);
        Luos_SendMsg(container, msg);
    }
    // target Rotation speed
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "target_rot_speed")))
    {
        // this should be a function because it is frequently used
        angular_speed_t angular_speed = (angular_speed_t)cJSON_GetObjectItem(jobj, "target_rot_speed")->valuedouble;
        AngularOD_SpeedToMsg(&angular_speed, msg);
        Luos_SendMsg(container, msg);
    }
    // target linear position
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "target_trans_position")))
    {
        linear_position_t linear_position = LinearOD_PositionFrom_mm((float)cJSON_GetObjectItem(jobj, "target_trans_position")->valuedouble);
        LinearOD_PositionToMsg(&linear_position, msg);
        Luos_SendMsg(container, msg);
    }
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "target_trans_position")))
    {
        int i = 0;
        // this is a trajectory
        item = cJSON_GetObjectItem(jobj, "target_trans_position");
        int size = (int)cJSON_GetArrayItem(item, 0)->valueint;
        // find the first \r of the current buf
        for (i = 0; i < JSON_BUFF_SIZE; i++)
        {
            if (bin_data[i] == '\r')
            {
                i++;
                break;
            }
        }
        if (i < JSON_BUFF_SIZE - 1)
        {
            msg->header.cmd = LINEAR_POSITION;
            // todo WATCHOUT this could be mm !
            Luos_SendData(container, msg, &bin_data[i], (unsigned int)size);
        }
    }
    // target Linear speed
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "target_trans_speed")))
    {
        linear_speed_t linear_speed = LinearOD_Speedfrom_mm_s((float)cJSON_GetObjectItem(jobj, "target_trans_speed")->valuedouble);
        LinearOD_SpeedToMsg(&linear_speed, msg);
        Luos_SendMsg(container, msg);
    }
    // time
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "time")))
    {
        // this should be a function because it is frequently used
        time = TimeOD_TimeFrom_s((float)cJSON_GetObjectItem(jobj, "time")->valuedouble);
        TimeOD_TimeToMsg(&time, msg);
        Luos_SendMsg(container, msg);
    }
    // Compliance
    if (cJSON_IsBool(cJSON_GetObjectItem(jobj, "compliant")))
    {
        msg->data[0] = cJSON_IsTrue(cJSON_GetObjectItem(jobj, "compliant"));
        msg->header.cmd = COMPLIANT;
        msg->header.size = sizeof(char);
        Luos_SendMsg(container, msg);
    }
    // Pid
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "pid")))
    {
        float pid[3];
        item = cJSON_GetObjectItem(jobj, "pid");
        pid[0] = (float)cJSON_GetArrayItem(item, 0)->valuedouble;
        pid[1] = (float)cJSON_GetArrayItem(item, 1)->valuedouble;
        pid[2] = (float)cJSON_GetArrayItem(item, 2)->valuedouble;
        memcpy(&msg->data[0], pid, sizeof(asserv_pid_t));
        msg->header.cmd = PID;
        msg->header.size = sizeof(asserv_pid_t);
        Luos_SendMsg(container, msg);
    }
    // resolution
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "resolution")))
    {
        // this should be a function because it is frequently used
        data = (float)cJSON_GetObjectItem(jobj, "resolution")->valuedouble;
        memcpy(msg->data, &data, sizeof(data));
        msg->header.cmd = RESOLUTION;
        msg->header.size = sizeof(data);
        Luos_SendMsg(container, msg);
    }
    //offset
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "offset")))
    {
        // this should be a function because it is frequently used
        data = (float)cJSON_GetObjectItem(jobj, "offset")->valuedouble;
        memcpy(msg->data, &data, sizeof(data));
        msg->header.cmd = OFFSET;
        msg->header.size = sizeof(data);
        Luos_SendMsg(container, msg);
    }
    // reduction ratio
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "reduction")))
    {
        // this should be a function because it is frequently used
        data = (float)cJSON_GetObjectItem(jobj, "reduction")->valuedouble;
        memcpy(msg->data, &data, sizeof(data));
        msg->header.cmd = REDUCTION;
        msg->header.size = sizeof(data);
        Luos_SendMsg(container, msg);
    }
    // dimension (m)
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "dimension")))
    {
        linear_position_t linear_position = LinearOD_PositionFrom_mm((float)cJSON_GetObjectItem(jobj, "dimension")->valuedouble);
        LinearOD_PositionToMsg(&linear_position, msg);
        // redefine a specific message type.
        msg->header.cmd = DIMENSION;
        Luos_SendMsg(container, msg);
    }
    // voltage
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "volt")))
    {
        // this should be a function because it is frequently used
        voltage_t volt = (voltage_t)cJSON_GetObjectItem(jobj, "volt")->valuedouble;
        ElectricOD_VoltageToMsg(&volt, msg);
        Luos_SendMsg(container, msg);
    }
    // reinit
    if (cJSON_GetObjectItem(jobj, "reinit"))
    {
        msg->header.cmd = REINIT;
        msg->header.size = 0;
        Luos_SendMsg(container, msg);
    }
    // control (play, pause, stop, rec)
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "control")))
    {
        msg->data[0] = cJSON_GetObjectItem(jobj, "control")->valueint;
        msg->header.cmd = CONTROL;
        msg->header.size = sizeof(control_t);
        Luos_SendMsg(container, msg);
    }
    // Color
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "color")))
    {
        item = cJSON_GetObjectItem(jobj, "color");
        int size = cJSON_GetArraySize(item);
        if (size == 3)
        {
            color_t color;
            for (int i = 0; i < 3; i++)
            {
                color.unmap[i] = (char)cJSON_GetArrayItem(item, i)->valueint;
            }
            IlluminanceOD_ColorToMsg(&color, msg);
            Luos_SendMsg(container, msg);
        }
        else
        {
            int i = 0;
            // This is a binary
            size = (int)cJSON_GetArrayItem(item, 0)->valueint;
            // find the first \r of the current buf
            for (i = 0; i < JSON_BUFF_SIZE; i++)
            {
                if (bin_data[i] == '\r')
                {
                    i++;
                    break;
                }
            }
            if (i < JSON_BUFF_SIZE - 1)
            {
                msg->header.cmd = COLOR;
                Luos_SendData(container, msg, &bin_data[i], (unsigned int)size);
            }
        }
    }
    // IO_STATE
    if (cJSON_IsBool(cJSON_GetObjectItem(jobj, "io_state")))
    {
        msg->data[0] = cJSON_IsTrue(cJSON_GetObjectItem(jobj, "io_state"));
        msg->header.cmd = IO_STATE;
        msg->header.size = sizeof(char);
        Luos_SendMsg(container, msg);
    }
    // UUID
    if (cJSON_GetObjectItem(jobj, "uuid"))
    {
        msg->header.cmd = NODE_UUID;
        msg->header.size = 0;
        Luos_SendMsg(container, msg);
    }
    // RENAMING
    if (cJSON_IsString(cJSON_GetObjectItem(jobj, "rename")))
    {
        // In this case we need to send the message as system message
        int i = 0;
        char *alias = cJSON_GetStringValue(cJSON_GetObjectItem(jobj, "rename"));
        msg->header.size = strlen(alias);
        // Change size to fit into 16 characters
        if (msg->header.size > 15)
        {
            msg->header.size = 15;
        }
        // Clean the '\0' even if we short the alias
        alias[msg->header.size] = '\0';
        // Copy the alias into the data field of the message
        for (i = 0; i < msg->header.size; i++)
        {
            msg->data[i] = alias[i];
        }
        msg->data[msg->header.size] = '\0';
        msg->header.cmd = WRITE_ALIAS;
        Luos_SendMsg(container, msg);
    }
    // FIRMWARE REVISION
    if (cJSON_GetObjectItem(jobj, "revision"))
    {
        msg->header.cmd = REVISION;
        msg->header.size = 0;
        Luos_SendMsg(container, msg);
    }
    // Luos REVISION
    if (cJSON_GetObjectItem(jobj, "luos_revision"))
    {
        msg->header.cmd = LUOS_REVISION;
        msg->header.size = 0;
        Luos_SendMsg(container, msg);
    }
    // Luos STAT
    if (cJSON_GetObjectItem(jobj, "luos_statistics"))
    {
        msg->header.cmd = LUOS_STATISTICS;
        msg->header.size = 0;
        Luos_SendMsg(container, msg);
    }
    // Parameters
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "parameters")))
    {
        uint32_t val = cJSON_GetObjectItem(jobj, "parameters")->valueint;
        memcpy(msg->data, &val, sizeof(uint32_t));
        msg->header.size = 4;
        msg->header.cmd = PARAMETERS;
        Luos_SendMsg(container, msg);
    }
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "parameters")))
    {
        item = cJSON_GetObjectItem(jobj, "parameters");
        int size = cJSON_GetArraySize(item);
        // find the first \r of the current buf
        for (int i = 0; i < size; i++)
        {
            uint32_t val = (uint32_t)cJSON_GetArrayItem(item, i)->valuedouble;
            memcpy(&msg->data[i * sizeof(uint32_t)], &val, sizeof(uint32_t));
        }
        msg->header.cmd = PARAMETERS;
        msg->header.size = size * sizeof(uint32_t);
        Luos_SendMsg(container, msg);
    }
    if (cJSON_IsArray(cJSON_GetObjectItem(jobj, "register"))) // Watch out this one is only used by Dxl and specific to it.
    {
        item = cJSON_GetObjectItem(jobj, "parameters");
        int size = cJSON_GetArraySize(item);
        // find the first \r of the current buf
        for (int i = 0; i < size; i++)
        {
            uint32_t val = (uint32_t)cJSON_GetArrayItem(item, 0)->valuedouble;
            memcpy(&msg->data[i * sizeof(uint32_t)], &val, sizeof(uint32_t));
        }
        msg->header.cmd = REGISTER;
        msg->header.size = size * sizeof(uint32_t);
        Luos_SendMsg(container, msg);
    }
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "set_id")))
    {
        msg->data[0] = (char)(cJSON_GetObjectItem(jobj, "set_id")->valueint);
        msg->header.cmd = SETID;
        msg->header.size = sizeof(char);
        Luos_SendMsg(container, msg);
    }
    if (cJSON_IsNumber(cJSON_GetObjectItem(jobj, "delay")))
    {
        // This one is for Gate
        set_delay(cJSON_GetObjectItem(jobj, "delay")->valueint);
    }
}

// Create Json from a container msg
void msg_to_json(msg_t *msg, char *json)
{
    switch (msg->header.cmd)
    {
    case LINEAR_POSITION:
    case LINEAR_SPEED:
    case ANGULAR_POSITION:
    case ANGULAR_SPEED:
    case VOLTAGE:
    case CURRENT:
    case POWER:
    case ILLUMINANCE:
    case TEMPERATURE:
    case FORCE:
    case MOMENT:
        // check size
        if (msg->header.size == sizeof(float))
        {
            // Size ok, now fill the struct from msg data
            float data;
            memcpy(&data, msg->data, msg->header.size);
            switch (msg->header.cmd)
            {
            case LINEAR_POSITION:
                sprintf(json, "\"trans_position\":%.3f,", data);
                break;
            case LINEAR_SPEED:
                sprintf(json, "\"trans_speed\":%.3f,", data);
                break;
            case ANGULAR_POSITION:
                sprintf(json, "\"rot_position\":%.3f,", data);
                break;
            case ANGULAR_SPEED:
                sprintf(json, "\"rot_speed\":%.3f,", data);
                break;
            case CURRENT:
                sprintf(json, "\"current\":%.3f,", data);
                break;
            case ILLUMINANCE:
                sprintf(json, "\"lux\":%.3f,", data);
                break;
            case TEMPERATURE:
                sprintf(json, "\"temperature\":%.3f,", data);
                break;
            case FORCE:
                sprintf(json, "\"force\":%.3f,", data);
                break;
            case MOMENT:
                sprintf(json, "\"moment\":%.3f,", data);
                break;
            case VOLTAGE:
                sprintf(json, "\"volt\":%.3f,", data);
                break;
            case POWER:
                sprintf(json, "\"power\":%.3f,", data);
                break;
            }
        }
        break;
    case NODE_UUID:
        if (msg->header.size == sizeof(luos_uuid_t))
        {
            luos_uuid_t value;
            memcpy(value.unmap, msg->data, msg->header.size);
            sprintf(json, "\"uuid\":[%" PRIu32 ",%" PRIu32 ",%" PRIu32 "],", value.uuid[0], value.uuid[1], value.uuid[2]);
        }
        break;
    case REVISION:
        // clean data to be used as string
        if (msg->header.size < MAX_DATA_MSG_SIZE)
        {
            msg->data[msg->header.size] = '\0';
            //create the Json content
            sprintf(json, "\"revision\":\"%d.%d.%d\",", msg->data[0], msg->data[1], msg->data[2]);
        }
        break;
    case LUOS_REVISION:
        // clean data to be used as string
        if (msg->header.size < MAX_DATA_MSG_SIZE)
        {
            msg->data[msg->header.size] = '\0';
            //create the Json content
            sprintf(json, "\"luos_revision\":\"%d.%d.%d\",", msg->data[0], msg->data[1], msg->data[2]);
        }
        break;
    case LUOS_STATISTICS:
        if (msg->header.size == sizeof(general_stats_t))
        {
            general_stats_t *stat = (general_stats_t *)msg->data;
            // create the Json content
            sprintf(json, "\"luos_statistics\":{\"msg_stack\":%d,\"luos_stack\":%d,\"msg_drop\":%d,\"loop_ms\":%d,\"fail_ratio\":%d,\"collision_max\":%d,\"nak_max\":%d},",
                    stat->node_stat.memory.msg_stack_ratio,
                    stat->node_stat.memory.luos_stack_ratio,
                    stat->node_stat.memory.msg_drop_number,
                    stat->node_stat.max_loop_time_ms,
                    stat->container_stat.msg_fail_ratio,
                    stat->container_stat.max_collision_retry,
                    stat->container_stat.max_nak_retry);
        }
        break;
    case IO_STATE:
        // check size
        if (msg->header.size == sizeof(char))
        {
            // Size ok, now fill the struct from msg data
            //create the Json content
            if (msg->data[0])
            {
                sprintf(json, "\"io_state\":true,");
            }
            else
            {
                sprintf(json, "\"io_state\":false,");
            }
        }
        break;
    case EULER_3D:
    case COMPASS_3D:
    case GYRO_3D:
    case ACCEL_3D:
    case LINEAR_ACCEL:
    case GRAVITY_VECTOR:
        // check size
        if (msg->header.size == (3 * sizeof(float)))
        {
            // Size ok, now fill the struct from msg data
            float value[3];
            memcpy(value, msg->data, msg->header.size);
            char name[20] = {0};
            switch (msg->header.cmd)
            {
            case LINEAR_ACCEL:
                strcpy(name, "linear_accel");
                break;
            case GRAVITY_VECTOR:
                strcpy(name, "gravity_vector");
                break;
            case COMPASS_3D:
                strcpy(name, "compass");
                break;
            case GYRO_3D:
                strcpy(name, "gyro");
                break;
            case ACCEL_3D:
                strcpy(name, "accel");
                break;
            case EULER_3D:
                strcpy(name, "euler");
                break;
            }
            //create the Json content
            sprintf(json, "\"%s\":[%2f,%2f,%2f],", name, value[0], value[1], value[2]);
        }
        break;
    case QUATERNION:
        // check size
        if (msg->header.size == (4 * sizeof(float)))
        {
            // Size ok, now fill the struct from msg data
            float value[4];
            memcpy(value, msg->data, msg->header.size);
            //create the Json content
            sprintf(json, "\"quaternion\":[%2f,%2f,%2f,%2f],", value[0], value[1], value[2], value[3]);
        }
        break;
    case ROT_MAT:
        // check size
        if (msg->header.size == (9 * sizeof(float)))
        {
            // Size ok, now fill the struct from msg data
            float value[9];
            memcpy(value, msg->data, msg->header.size);
            //create the Json content
            sprintf(json, "\"rotational_matrix\":[%2f,%2f,%2f,%2f,%2f,%2f,%2f,%2f,%2f],", value[0], value[1], value[2], value[3], value[4], value[5], value[6], value[7], value[8]);
        }
        break;
    case HEADING:
        // check size
        if (msg->header.size == (sizeof(float)))
        {
            // Size ok, now fill the struct from msg data
            float value;
            memcpy(&value, msg->data, msg->header.size);
            //create the Json content
            sprintf(json, "\"heading\":%2f,", value);
        }
        break;
    case PEDOMETER:
        // check size
        if (msg->header.size == (2 * sizeof(unsigned long)))
        {
            // Size ok, now fill the struct from msg data
            unsigned long value[2];
            memcpy(value, msg->data, msg->header.size);
            //create the Json content
            sprintf(json, "\"pedometer\":%2ld,\"walk_time\":%2ld,", value[0], value[1]);
        }
        break;
    default:
        break;
    }
}

void routing_table_to_json(char *json)
{
    // Init the json string
    char *json_ptr = json;
    sprintf(json_ptr, "{\"routing_table\":[");
    json_ptr += strlen(json_ptr);
    // loop into containers.
    routing_table_t *routing_table = RoutingTB_Get();
    int last_entry = RoutingTB_GetLastEntry();
    int i = 0;
    //for (uint8_t i = 0; i < last_entry; i++) { //TODO manage all entries, not only containers.
    while (i < last_entry)
    {
        if (routing_table[i].mode == NODE)
        {
            sprintf(json_ptr, "{\"node_id\":%d", routing_table[i].node_id);
            json_ptr += strlen(json_ptr);
            if (routing_table[i].certified)
            {
                sprintf(json_ptr, ",\"certified\":true");
                json_ptr += strlen(json_ptr);
            }
            else
            {
                sprintf(json_ptr, ",\"certified\":false");
                json_ptr += strlen(json_ptr);
            }
            sprintf(json_ptr, ",\"port_table\":[");
            json_ptr += strlen(json_ptr);
            // Port loop
            for (int port = 0; port < 4; port++)
            {
                if (routing_table[i].port_table[port])
                {
                    sprintf(json_ptr, "%d,", routing_table[i].port_table[port]);
                    json_ptr += strlen(json_ptr);
                }
                else
                {
                    // remove the last "," char
                    *(--json_ptr) = '\0';
                    break;
                }
            }
            sprintf(json_ptr, "],\"containers\":[");
            json_ptr += strlen(json_ptr);
            i++;
            // Containers loop
            while (i < last_entry)
            {
                if (routing_table[i].mode == CONTAINER)
                {
                    // Create container description
                    sprintf(json_ptr, "{\"type\":\"%s\",\"id\":%d,\"alias\":\"%s\"},", RoutingTB_StringFromType(routing_table[i].type), routing_table[i].id, routing_table[i].alias);
                    json_ptr += strlen(json_ptr);
                    i++;
                }
                else
                    break;
            }
            // remove the last "," char
            *(--json_ptr) = '\0';
            sprintf(json_ptr, "]},");
            json_ptr += strlen(json_ptr);
            // Check if we overflow the Json buffer
            // We can do it because we know that we are at the begining off buffer bank
            if ((json_ptr - json) >= JSON_BUFF_SIZE)
            {
                // we override it, start sending the first part
                json = json_alloc_set_tx_task(JSON_BUFF_SIZE - 1);
            }
        }
        else
        {
            i++;
        }
    }
    // remove the last "," char
    *(--json_ptr) = '\0';
    // End the Json message
    sprintf(json_ptr, "]}\n");
    json = json_alloc_set_tx_task(strlen(json));
}

void exclude_container_to_json(int id, char *json)
{
    sprintf(json, "%s", "{\"dead_container\": ");
    sprintf(json, "%s\"%s\"", json, RoutingTB_AliasFromId(id));
    sprintf(json, "%s}\n", json);
    RoutingTB_RemoveOnRoutingTable(id);
    json = json_alloc_set_tx_task(strlen(json));
}

void node_assert(char *file, uint32_t line)
{
    // manage self crashing scenario
    char *json = json_alloc_get_tx_buf();
    sprintf(json, "{\"assert\":{\"node_id\":1,\"file\":\"%s\",\"line\":%d}}\n", file, (unsigned int)line);
    json_alloc_set_tx_task(strlen(json));
}