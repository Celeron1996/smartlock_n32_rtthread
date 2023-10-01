#ifndef FM01_DRV_H
#define FM01_DRV_H

#include "fm01_bsp.h"


#define FM01_RX_BUFFER_SIZE		(32u)

#define FM01_1000_MS_FIX		(1000u)
#define FM01_RX_TIMEOUT_MS		((RT_TICK_PER_SECOND * 1000) / FM01_1000_MS_FIX)


//AS60x指令集
typedef enum
{
	CM_GetImage 							= 0x01,	//验证用获取图像
	CM_GenChar 								= 0x02,	//根据原始图像生成指纹特征存于特征文件缓冲区
	CM_Match 									= 0x03,	//精确比对特征文件缓冲区中的特征文件
	CM_Search 								= 0x04,	//以特征文件缓冲区中的特征文件搜索整个或部分指纹库
	CM_RegModel 							= 0x05,	//将特征文件合并生成模板存于特征文件缓冲区
	CM_StoreChar 							= 0x06,	//将特征缓冲区中的文件储存到flash指纹库中
	CM_LoadChar							  = 0x07,	//从flash指纹库中读取一个模板到特征缓冲区
	CM_UpChar 								= 0x08,	//将特征缓冲区中的文件上传给上位机
	CM_DownChar							  = 0x09,	//从上位机下载一个特征文件到特征缓冲区
	CM_UpImage 								= 0x0A,	//上传原始图像
	CM_DownImage 							= 0x0B,	//下载原始图像
	CM_DeletChar 							= 0x0C,	//删除flash指纹库中的一个特征文件
	CM_Empty 									= 0x0D,	//清空flash指纹库
	CM_WriteReg 							= 0x0E,	//写SOC系统寄存器
	CM_ReadSysPara					  = 0x0F,	//读系统基本参数
	CM_SetPwd								  = 0x12,	//设置设备握手口令
	CM_VfyPwd								  = 0x13,	//验证设备握手口令
	CM_GetRandomCode				  = 0x14,	//采样随机数
	CM_SetChipAddr					  = 0x15,	//设置芯片地址
	CM_ReadINFpage 						= 0x16,	//读取FLASH Information Page内容
	CM_Port_Control					  = 0x17,	//通讯端口(UART/USB)开关控制
	CM_WriteNotepad 					= 0x18,	//写记事本
	CM_ReadNotepad 						= 0x19,	//读记事本
	CM_BurnCode 							= 0x1A,	//烧写片内FLASH
	CM_HighSpeedSearch 				= 0x1B,	//高速搜索FLASH
	CM_GenBinImage 						= 0x1C,	//生成二值化指纹图像
	CM_ValidTempleteNum 			= 0x1D,	//读有效模板个数
	CM_UserGPIOCommand 				= 0x1E,	//用户GPIO控制命令
	CM_ReadIndexTable 				= 0x1F,	//读索引表
	CM_GetEnrollImage 				= 0x29,	//注册用获取图像
	CM_Cancle 								= 0x30,	//取消指令
	CM_AutoEnroll 						= 0x31,	//自动注册模板指令
	CM_AutoIdentify 					= 0x32,	//自动验证指纹指令
	CM_Sleep 									= 0x33,	//休眠指令
	CM_GetChiCMN 							= 0x34,	//读取芯片唯一序列号
	CM_HandShake 							= 0x35,	//握手指令
	CM_CheckSensor 						= 0x36	//校验传感器
} FP_Command_TypeDef;


//指令应答
typedef enum
{
	RS_SUCCESS 								= 0x00,	//表示指令执行完毕或OK
	RS_DataError 							= 0x01,	//表示数据包接收错误
	RS_NoFinger 							= 0x02,	//表示传感器上没有手指
	RS_ImageError 						= 0x03,	//表示录入指纹图像失败
	RS_ImageDefective0 				= 0x04,	//表示指纹图像太干、太淡而生不成特征
	RS_ImageDefective1 				= 0x05,	//表示指纹图像太湿、太糊而生不成特征
	RS_ImageDefective2 				= 0x06,	//表示图像太乱而生不成特征
	RS_CharFew 								= 0x07,	//表示图像正常，但特征点太少（或面积太少）而生不成特征
	RS_FMismatch 							= 0x08,	//表示指纹不匹配
	RS_FNoSearch 							= 0x09,	//表示没搜索到指纹
	RS_RegModelError 					= 0x0A,	//表示特征合并失败
	RS_FlashCross 						= 0x0B,	//表示访问指纹库时地址序号超出指纹库范围
	RS_FlashError 						= 0x0C,	//表示从指纹库读取模板出错或无效
	RS_UpCharError 						= 0x0D,	//表示上传特征失败
	RS_ReceiveError 					= 0x0E,	//表示模块不能接收后续数据包
	RS_UpImageError 					= 0x0F,	//表示上传图像失败
	RS_DeletCharError 				= 0x10,	//表示删除模板失败
	RS_EmptyError 						= 0x11,	//表示清空指纹库失败
	RS_SleepError 						= 0x12,	//表示不能进入低功耗状态
	RS_PwdError 							= 0x13,	//表示口令不正确
	RS_ResetError 						= 0x14,	//表示系统复位失败
	RS_BuffNoImage 						= 0x15,	//表示缓冲区内没有有效原始图而生不成图像
	RS_UpDateError 						= 0x16,	//表示在线升级失败
	RS_FingerNoMove 					= 0x17,	//表示残留的指纹或两次采集之间手指没有移动过
	RS_RWFlashError 					= 0x18,	//表示读写FLASH出错
	RS_RandomCode_Error 			= 0x19,	//随机数生成失败
	RS_RegNumber_Error 				= 0x1A,	//无效寄存器号
	RS_RegValue_Error 				= 0x1B,	//寄存器设定内容错误号
	RS_NotePage_Error 				= 0x1C,	//笔记本页码指定错误
	RS_Port_Error 						= 0x1D,	//端口操作失败
	RS_AutoEnroll_Error 			= 0x1E,	//自动注册失败
	RS_Flash_Full 						= 0x1F,	//指纹库满
	RS_DeviAddr_Error 				= 0x20,	//设备地址错误
	RS_PassError 							= 0x21,	//密码有误
	RS_FModelNoEmpty				  = 0x22,	//指纹模板非空
	RS_FModelEmpty 						= 0x23,	//指纹模板为空
	RS_Flash_Empty 						= 0x24,	//指纹库为空
	RS_EnrCount_Error 				= 0x25,	//录入次数设置错误
	RS_TimeOut 								= 0x26,	//超时
	RS_FingerIsExist 					= 0x27,	//指纹已存在
	RS_FmodelParallel 				= 0x28,	//指纹模板有并联
	RS_SensorInit_Error 			= 0x29,	//传感器初始化失败
	RS_SUCCESS_DATA 					= 0xF0,	//有后续数据包的指令，正确接收后用0xF0应答
	RS_SUCCESS_COMD 					= 0xF1,	//有后续数据包的指令，命令包用0xF1应答
	RS_WFlash_SumError 				= 0xF2,	//表示烧写内部FLASH时，校验和错误
	RS_WFlash_IdeError 				= 0xF3,	//表示烧写内部FLASH时，包标识错误
	RS_WFlash_LenError 				= 0xF4,	//表示烧写内部FLASH时，包长度错误
	RS_WFlash_CodError 				= 0xF5,	//表示烧写内部FLASH时，代码长度太长
	RS_WFlash_Error 					= 0xF6,	//表示烧写内部FLASH时，烧写FLASH失败
	
	RS_OtherError							= 0x2A	//这是自定义的类型，AS608并不存在这个应答指令！
																		//因为0x2A-0xEF在AS608中是保留的，没被使用
}	FP_Response_TypeDef;



typedef enum {
	CM_GetImage_RxSize			= 12,
	CM_GetEnrollImage_RxSize	= 12,
	CM_Sleep_RxSize				= 12,
	CM_GenChar_RxSize			= 12,
	CM_Search_RxSize			= 16,
	CM_RegModel_RxSize			= 12,
	CM_StoreChar_RxSize			= 12,
	CM_DeletChar_RxSize			= 12,
	CM_Empty_RxSize				= 12
} FP_Cmd_Rx_Size;


rt_err_t fm01_get_image(void);
rt_err_t fm01_get_enroll_image(void);
rt_err_t fm01_sleep(void);
rt_err_t fm01_gen_char(uint8_t buffer_id);
rt_err_t fm01_search(uint8_t buffer_id, uint16_t start_page, uint16_t page_num, uint16_t *user_num);
rt_err_t fm01_reg_model(void);
rt_err_t fm01_store_model(uint8_t buffer_id, uint16_t page_id);
rt_err_t fm01_delete_char(uint16_t page_id, uint16_t num);
rt_err_t fm01_empty(void);


#endif

