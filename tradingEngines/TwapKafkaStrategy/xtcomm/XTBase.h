
#ifndef _XTBASE_DEFINE_H
#define _XTBASE_DEFINE_H

//..............................................................................................................................................................


//交易所
#define		XT_EXCH_UNKONW			0x00
#define		XT_EXCH_SSE				0x01
#define		XT_EXCH_SZSE			0x02

#define		XT_EXCH_CFFEX			0x04
#define		XT_EXCH_DCE				0x08
#define		XT_EXCH_CZCE			0x10
#define		XT_EXCH_SHFE			0x20
#define		XT_EXCH_INE				0x40



//交易所名称
#define		XT_EXCHNAME_SSE			"SSE"
#define		XT_EXCHNAME_SZSE		"SZSE"
#define		XT_EXCHNAME_CFFEX		"CFFEX"
#define		XT_EXCHNAME_DCE			"DCE"
#define		XT_EXCHNAME_CZCE		"CZCE"
#define		XT_EXCHNAME_SHFE		"SHFE"
#define		XT_EXCHNAME_INE         "INE"

//交易所名称 中文名
#define		XT_EXCHCHNNAME_SSE		"上海证券交易所"
#define		XT_EXCHCHNNAME_SZSE		"深圳证券交易所"
#define		XT_EXCHCHNNAME_CFFEX	"中金所"
#define		XT_EXCHCHNNAME_DCE      "大商所"
#define		XT_EXCHCHNNAME_CZCE		"郑商所"
#define		XT_EXCHCHNNAME_SHFE		"上期所"
#define     XT_EXCHCHNNAME_INE      "能源中心"

//..............................................................................................................................................................
//商品类型
#define		XT_GROUP_FUTURE			1
#define		XT_GROUP_OPTION			2

//#define		XT_GROUP_INDEX			0
//#define		XT_GROUP_STOCKA			2
//#define		XT_GROUP_STOCKB			3
//#define		XT_GROUP_ETF			4

//..............................................................................................................................................................
//账户类型
#define		XT_UT_STOCK				1		
#define		XT_UT_MARGIN			2
#define		XT_UT_STOCKOPT			3
#define		XT_UT_FUTUREOPT			4


//..............................................................................................................................................................
//期权合约类型
#define		XT_OPTION_CALL			'0'
#define		XT_OPTION_PUT			'1'

//..............................................................................................................................................................
#pragma pack(1)

//商品
typedef struct  
{
	unsigned char			ExchID;
	char					Code[31];
}tagXTInstrument, tagXTCommodity;


#define		d4_5_					0.500001				//四舍五入校正值 0.5是为了四舍五入，0.000001是为了校正浮点数
#define		SHOPT_MULTI				(10000.0)				//上海期权的价格放大倍数
#define		SHOPT_NPRICE(d)         (int)(d * 10000.0 + d4_5_)		//把执行价，最新价等放大10000倍取整


#pragma pack()

//..............................................................................................................................................................









#endif
