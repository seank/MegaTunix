/*
 * Copyright (C) 2002-2011 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute, etc. this as long as all the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#ifndef __ENUMS_H__
#define __ENUMS_H__

/* Runtime Status flags */
typedef enum 
{       
	STAT_CONNECTED = 0, 
        STAT_CRANKING, 
        STAT_RUNNING, 
        STAT_WARMUP, 
        STAT_AS_ENRICH, 
        STAT_ACCEL, 
        STAT_DECEL
}RuntimeStatus;

typedef enum
{
	VE_ = 0xb0,
	VE_IMPORT,
	DATALOG_INT_DUMP,
	DATALOG_,
	DATALOG_IMPORT,
	FIRMWARE_LOAD,
	FULL_BACKUP,
	FULL_RESTORE
}FileIoType;


typedef enum
{	
	RED=0xc0,
	BLACK,
	GREEN,
	BLUE
}GuiColor;

typedef enum
{
	FONT=0x100,
	TRACE,
	GRATICULE,
	HIGHLIGHT,
	TTM_AXIS,
	TTM_TRACE
}GcType;

typedef enum
{
	WRITE_CMD = 0x130,
	NULL_CMD,
	FUNC_CALL
}CmdType;

typedef enum
{
	UPD_LOGBAR=0x180,
	UPD_WIDGET,
/*	UPD_RAW_MEMORY,*/
	UPD_RUN_FUNCTION,
	UPD_REFRESH,
	UPD_REFRESH_RANGE
}UpdateFunction;

typedef enum
{
	MTX_INT = 0x190,
	MTX_ENUM,
	MTX_BOOL,
	MTX_FLOAT,
	MTX_STRING,
	MTX_UNKNOWN
}DataType;

typedef enum
{
	ABOUT_TAB=0x1a0,
	GENERAL_TAB,
	COMMS_TAB,
	ENG_VITALS_TAB,
	CONSTANTS_TAB,
	DT_PARAMS_TAB,
	IGNITON_TAB,
	RUNTIME_TAB,
	ACCEL_WIZ_TAB,
	ENRICHMENTS_TAB,
	TUNING_TAB,
	TOOLS_TAB,
	RAW_MEM_TAB,
	WARMUP_WIZ_TAB,
	VETABLES_TAB,
	SPARKTABLES_TAB,
	AFRTABLES_TAB,
	ALPHA_N_TAB,
	BOOSTTABLES_TAB,
	ROTARYTABLES_TAB,
	DATALOGGING_TAB,
	LOGVIEWER_TAB,
	VE3D_VIEWER_TAB,
	ERROR_STATUS_TAB,
	SETTINGS_TAB,
	CORRECTIONS_TAB,
	STAGING_TAB
}TabIdent;

typedef enum
{
	ECU_EMB_BIT=0x1c0,
	ECU_VAR,
	RAW_EMB_BIT,
	RAW_VAR
}ComplexExprType;

typedef enum
{
	UPLOAD=0x1c8,
	DOWNLOAD,
	RTV,
	GAUGE
}ConvType;

typedef enum
{
	MTX_HEX=0x1d0,
	MTX_DECIMAL
}Base;

typedef enum
{
	MTX_ENTRY=0x1e0,
	MTX_TITLE,
	MTX_LABEL,
	MTX_RANGE,
	MTX_SPINBUTTON,
	MTX_PROGRESS,
	MTX_SCALE,
	MTX_SENSITIVE
}WidgetType;


typedef enum
{
	RTV_TICKLER=0x210,
	LV_PLAYBACK_TICKLER,
	SCOUNTS_TICKLER
}TicklerType;

typedef enum
{
	ALPHA_N=0x220,
	SPEED_DENSITY,
	MAF,
	SD_AN_HYBRID,
	MAF_AN_HYBRID,
	SD_MAF_HYBRID
}Algorithm;

typedef enum
{
	MTX_SIMPLE_WRITE=0x240,
	MTX_CHUNK_WRITE,
	MTX_CMD_WRITE
}WriteMode;


typedef enum
{
	MTX_CHAR=0x250,
	MTX_U08,
	MTX_S08,
	MTX_U16,
	MTX_S16,
	MTX_U32,
	MTX_S32,
	MTX_UNDEF
}DataSize;

typedef enum
{
	LV_PLAYBACK=0x270,
	LV_REALTIME
}Lv_Mode;

typedef enum
{
	LV_GOTO_START=0x280,
	LV_GOTO_END,
	LV_REWIND,
	LV_FAST_FORWARD,
	LV_STOP,
	LV_PLAY
}Lv_Handler;

typedef enum
{
	DATA=0x2A0,
	ACTION,
	STATIC_STRING,
	LAST_ARG_TYPE
}ArgType;

typedef enum
{
	SLEEP=0x2C0
}Action;

typedef enum
{
	FUNCTIONS=0x2D0,
	POST_FUNCTIONS,
	SEQUENCE,
	ARGS
}ArrayType;

typedef enum
{
	ADD=0,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	EQUAL
}ScaleOp;

typedef enum
{
	DEP_TYPE=0,
	DEP_SIZE,
	DEP_PAGE,
	DEP_OFFSET,
	DEP_BITMASK,
	DEP_BITSHIFT,
	DEP_BITVAL
}DepVector;

typedef enum
{
	_X_ = 0x420,
	_Y_,
	_Z_
}Axis;

typedef enum
{
        CLT=0x430,
        IAT
}SensorType;


typedef enum
{
	LOWER=0x440,
	UPPER
}Extreme;

typedef enum
{
	OR=0x450,
	AND
}MatchType;

typedef enum
{
	MTX_DATA_CHANGED=0x460,
	MTX_STATUS_CHANGED
}SlaveMsgType;

typedef enum
{
	GROUP_SET_COLOR=0x01
}RemoteAction;

typedef enum
{
	SER_INTERVAL_DELAY = 0x480,
	SER_READ_TIMEOUT,
	RTSLIDER_FPS,
	RTTEXT_FPS,
	DASHBOARD_FPS,
	VE3D_FPS,
	SET_SER_PORT,
	LOGVIEW_ZOOM,
	DEBUG_LEVEL,
	GENERIC,
	BAUD_CHANGE,
	MS2_USER_OUTPUTS,
	LAST_BUTTON_ENUM
}MtxButton;

/* Toggle/Radio buttons */
typedef enum
{
	TOOLTIPS_STATE=0x500,
	LOG_RAW_DATASTREAM,
	TRACKING_FOCUS,
	COMMA,
	TAB,
	REALTIME_VIEW,
	PLAYBACK_VIEW,
	OFFLINE_FIRMWARE_CHOICE,
	ECU_PERSONA,
	COMM_AUTODETECT,
	TOGGLE_NETMODE,
	LAST_TOGGLE_BUTTON_ENUM
}ToggleButton;


typedef enum
{
	START_REALTIME = 0x520,
	STOP_REALTIME,
	START_PLAYBACK,
	STOP_PLAYBACK,
	READ_VE_CONST,
	READ_RAW_MEMORY,
	BURN_MS_FLASH,
	INTERROGATE_ECU,
	OFFLINE_MODE,
	SELECT_DLOG_EXP,
	SELECT_DLOG_IMP,
	DLOG_SELECT_DEFAULTS,
	DLOG_SELECT_ALL,
	DLOG_DESELECT_ALL,
	DLOG_DUMP_INTERNAL,
	CLOSE_LOGFILE,
	START_DATALOGGING,
	STOP_DATALOGGING,
	EXPORT_VETABLE,
	IMPORT_VETABLE,
	REBOOT_GETERR,
	REVERT_TO_BACKUP,
	BACKUP_ALL,
	RESTORE_ALL,
	SELECT_PARAMS,
	RESCALE_TABLE,
	EXPORT_SINGLE_TABLE,
	IMPORT_SINGLE_TABLE,
	TE_TABLE,
	TE_TABLE_GROUP,
	PHONE_HOME,
	LAST_STD_BUTTON_ENUM
}StdButton;

typedef enum
{
	KELVIN = 0x540,
	FAHRENHEIT,
	CELSIUS
}TempUnits;
#endif
