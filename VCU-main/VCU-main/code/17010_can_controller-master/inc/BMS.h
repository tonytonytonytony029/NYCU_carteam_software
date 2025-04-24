typedef enum{
    normal,
    err01,
    err02
}BMS_state;

typedef struct VCUBMSComm
{
    BMS_state state;

} BMS_t;



extern BMS_t Comm_dev;

