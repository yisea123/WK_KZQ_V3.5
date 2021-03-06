#include "config.h"
#include "control.h"
#include "usart.h"
#include "cmd.h"

//在这里配置系统类型

void Set_Addr (u8 addr)
{
	MY_ADDR=addr;
}

//设置本机控制器类型，返回0xff失败,0成功
u8 Set_MyStyle (u8 style)
{
	if(style>4||style<2)
		return 0xff;
	else
		MY_STYLE=style;
	return 0;
}




//设置本机所属采集器id，返回0xff失败,0成功
u8 Set_CJ_Addr (u16 addr)
{
	if ((addr&0xff00)==0xff00||(addr&0x00ff)==0x00ff||(addr==0x0000))
	{
		return 0xff;
	}
	else
		CJ_ADDR=addr;
	return 0;
}




char type_name[][4]={"kt","csj","jsj","jhq","ytj"};






#include "string.h"

//配置初始化，默认是空调
void Config_Init(void)
{
	 
									//加载flash里 的数据
	memset(DATA_MAP,0,1024);
	Load_Config( );

	memcpy(DATA_MAP->devType,"kzq",4);
	memcpy(DATA_MAP->softVersion,"3.5",4);
	
	DATA_MAP->outputNew=0;
	DATA_MAP->outputOld=0;
	
	
									//如果地址没有被配置过，初始化为1
	if (MY_ADDR>=0xffff)
	{
		MY_ADDR=0x01;
	}
	
									//如果控制器类型值在可选值之外，初始化为空调
	if (MY_STYLE>10||MY_STYLE<2)
	{
		MY_STYLE=MYSTYLE_KT;
	}
	
	memcpy(DATA_MAP->extType,type_name[MY_STYLE-2],4);

	
	
									//工作模式不正确，初始化为模式2
	if (WORK_MODE<=0||WORK_MODE>=3)
	{
		WORK_MODE=WORK_MODE2;					//默认是单纯控制模式
	}
	
	
													
																	//所属采集器地址不对，初始化为2
	if ((CJ_ADDR&0xff00)==0xff00||(CJ_ADDR&0x00ff)==0x00ff||(CJ_ADDR==0x0000))
	{
		CJ_ADDR=0x02;
	}
	
	
	
	
//		MY_ADDR=0x05;			//调试用
		WORK_MODE=WORK_MODE2;					//调试用，强制模式2
//		CJ_ADDR=0x02;//调试用
//		MY_STYLE=MYSTYLE_YT;
	
	
	
	
	
							//根据工作模式初始化一些参数
							//其中包括命令读取函数的指针，命令解析函数的指针
	if (WORK_MODE==WORK_MODE1)
	{
		RECV_IRQHandler=RECV_MODE1_IRQ;
		USART1_GetCmd=USART1_GetCmd_MODE1;
		//Get_cmd=Get_cmd_MODE1;
	}
	else if (WORK_MODE==WORK_MODE2)			//新的多条协议
	{
		RECV_IRQHandler=RECV_MODE1_IRQ;
				USART1_GetCmd=USART1_GetCmd_MODE2_1;
		Get_cmd=Get_cmd_NEW_MODE;
	}
	
	
	
	
			//必须重新初始化的变量
	
						//设备状态，全部为关
	KT_STATE=0;
	KT_STATE1=0;
	CS_STATE=0;
	JH_STATE=0;
	
						//配置状态，正常工作
	MY_MODEL=0;
	
						//一些属性
	IF_WORK=1;
	IF_HAND=2;
	
						//报警标志位
	WARN_WATER=0;
	WARN_WENDU=0;
	WARN_SHIDU=0;
	WARN_TVOC=0;
	WARN_PM2_5=0;
	CS_IN_CD=0;
	
//	control_run (MY_STYLE-1,1);//点亮自己属性的标志灯
}

