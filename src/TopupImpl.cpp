/*************************************************************************
	> File Name: TopupImpl.cpp
	> Author: desionwang
	> Mail: wdxin1322@qq.com 
	> Created Time: Sat 08 Feb 2014 04:16:03 PM CST
 ************************************************************************/

#include "TopupImpl.h"
#include "TopupUtils.h"
#include "HttpClient.h"
#include "GlobalConfig.h"
using namespace std;
using namespace  ::topupinterface;

TopupImpl::TopupImpl(){
	m_topup_info = new TopupInfo();
}

TopupImpl::~TopupImpl(){
	delete m_topup_info;
}

bool ChannelRank(ChannelInfo channelA, ChannelInfo channelB){
	return channelA.priority > channelB.priority;
}

int TopupImpl::Init(Connection *conn)
{
	m_conn = conn;
	return 0;
}


#define LAKE_PARAM_ERR		"0101"
#define NO_PRODUCT_ERR		"0305"
#define PRODUCT_MAIN_ERR	"0304"
#define SIGN_NOMATCH_ERR	"0102"
//处理FCGI的请求，根据请求的URI判断如何处理
int TopupImpl::HandleRequest(TopupServer *tpServer,const TopupRequest& request, string &result){
	TP_WRITE_LOG(tpServer, "[%s]\t%s\t%s\t%d", request.uri.c_str()
			,request.query.c_str(), request.checksum.c_str(), request.itimestamp);
	const char *params = request.query.c_str();
	const char *uri = request.uri.c_str();
	uint32_t request_time = request.itimestamp;
	const char *md5str = request.checksum.c_str();
	const char *m_interface = NULL;
	//post的各个参数
	//map<string, string, cmpKeyAscii> map_entitys;
	//解析post参数
	parse_params(params, &map_entitys);
	//解析query参数，并封装结构
	parse_query(params, m_topup_info);
	if(uri != NULL){
		m_interface = strrchr(uri, '/');
		if(m_interface == NULL){
			//TODO 输出错误
		}
	}
	//根据调用的URI参数判断调用的相应接口
	TP_WRITE_LOG(tpServer, "\t{%s}", m_interface);
	if(strcmp(m_interface + 1, "topup.fcg") == 0){
		//调用充值接口
		TmallCharge(tpServer, result);
	}else if(strcmp(m_interface, "query") == 0){
		//调用查询订单查询接口
	}else if(strcmp(m_interface, "cancel") == 0){
		//调用取消接口
	}else if(strcmp(m_interface, "balance") == 0){
		//调用余额查询接口
	}
#ifdef DEBUG
	map<string, string>::iterator it = map_entitys.begin();
	for(;it != map_entitys.end(); ++it){
			printf("key:%s\tvalue:%s\n", it->first.c_str(), it->second.c_str());
	}
#endif
	return 0;
}

///充值接口用于天猫和下游订购用户
int TopupImpl::TmallCharge(TopupServer *tpServer, string &response){
	//TODO 验证参数的正确性
	//http://host:port/resource?coopId=xxx&tbOrderNo=xxx&cardId=xxx&cardNum=xxx&customer=xxx&sum=xxx&gameId=xxx&section1=xxx&section2=xxx&notifyUrl=xxx&sign=xxx&version=xxx
	
	map<string, string>::iterator it;
	string coopId;			//商家编号
	string tbOrderNo;		//淘宝的订单号
	string cardId;			//充值卡商品编号
	int cardNum;			//充值卡数量
	string customer;		//手机号码
	double sum;				//本次充值总金额
	string tbOrderSnap;		//商品信息快照
	string notifyUrl;		//异同通知地址
	string sign;			//签名字符串
	string version;			//版本

	if(m_topup_info->coopId.empty()){
		MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) NO coopId %s", LAKE_PARAM_ERR);
		return 1;
	}
	if(m_topup_info->tbOrderNo.empty()){
		MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) NO tbOrderNo %s", LAKE_PARAM_ERR);
		return 1;
	}
	if(m_topup_info->cardId.empty()){
		MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) NO cardId %s", LAKE_PARAM_ERR);
		return 1;
	}
	if(m_topup_info->cardNum == 0){
		MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) NO cardNum %s", LAKE_PARAM_ERR);
		return 1;
	}
	if(m_topup_info->customer.empty()){
		MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) NO customer %s", LAKE_PARAM_ERR);
		return 1;
	}
	if(m_topup_info->sum == 0.){
		MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) NO sum %s", LAKE_PARAM_ERR);
		return 1;
	}
	if(m_topup_info->tbOrderSnap.empty()){
		MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) NO tbOrderSnap %s", LAKE_PARAM_ERR);
		return 1;
	}
	if(m_topup_info->sign.empty()){
		MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) NO sign %s", LAKE_PARAM_ERR);
		return 1;
	}

	if(!CheckSign()){
		MakeErrReplay(SIGN_NOMATCH_ERR, SORDER_FAILED, response);
		TP_WRITE_LOG(tpServer, "\t(TmallCharge) sign error %s", SIGN_NOMATCH_ERR);
		return 4;
	}
	//TODO 选择正确的产品，所有产品信息加入缓存，商品更新发送通知，重新加载缓存
	int check_product = CheckProduct();
	if(check_product == 2){
		MakeErrReplay(NO_PRODUCT_ERR, SORDER_FAILED, response);
		return 2;
	}else if(check_product == 1){
		MakeErrReplay(PRODUCT_MAIN_ERR, SORDER_FAILED, response);
		return 3;
	}
	//TODO 选择最优的渠道，渠道信息同样加入缓存，信息更新，重新加载
	int selectChannel = SelectBestChannel();
	if(selectChannel <= 0){
		MakeErrReplay(PRODUCT_MAIN_ERR, SORDER_FAILED, response);
		return 3;
	}
	//TODO 建立订单，订单创建采用append模式，快速，采用按天分表模式，保留一个月的数据
	int create_status = CreateTmallOrder();
	//TODO 返回结果
	MakeSuccessReplay(SUNDERWAY, response);
	return 0;
}


///天猫查询接口，用于查询订单
int TopupImpl::TmallQuery(TopupServer *tpServer, string &response){
	//验证参数的正确性
	map<string, string>::iterator it;
	string coopId;			//商家编号
	string tbOrderNo;		//淘宝的订单号
	string sign;			//签名字符串
	//查询订单
	//返回结果
	return 0;
}

//天猫回调接口，向TMALL发送回调请求，接口需要tmall和下游用户实现，该方法只发送回调请求
int TopupImpl::TmallNotify(TopupServer *tpServer, string &response){
	//向天猫或下游订购用户发送回调请求
	//验证返回结果，并且实现重发策略
	return 0;
}

///只用于接收处理天猫的取消请求
int TopupImpl::TmallCancel(TopupServer *tpServer, string &response){
	map<string, string>::iterator it;
	string coopId;			//商家编号
	string tbOrderNo;		//淘宝的订单号
	string sign;			//签名字符串
	return 0;
}

///用于处理下游订购用户的查询余额请求
int TopupImpl::GetBalance(string &response){
	//查询数据库，取得余额信息
	//返回余额信息
	return 0;
}

///回填充值结果，针对不同的请求，回填不同的xml结果
int TopupImpl::FillXMLResult(map<string, string> &params, RequestType req_type, int errno, string &result){
	//返回xml结果缓冲区
	char rest[2048] = {0};
	//充值返回结果
	if(req_type == CHARGE){
	
	}else if(req_type == QUERY){
		//查询订单返回结果
	}else if(req_type == CANCEL){
		//取消订单返回结果
	}else if(req_type == BALANCE){
		//查询余额返回结果
	}else if(req_type == NOTIFY){
		//接收回调通知返回结果
	}
	return 0;
}

//返回错误信息
int TopupImpl::MakeErrReplay(const char* errCode,const char* status, string &result){
	char buf[2048] = {0};
	int len = 0;
	len += sprintf(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	len += sprintf(buf + len, "<response>");
	len += sprintf(buf + len, "<tbOrderNo>%s</tbOrderNo>", m_topup_info->tbOrderNo.c_str());
	len += sprintf(buf + len, "<coopOrderNo>%lu</coopOrderNo>", m_topup_info->coopOrderNo);
	len += sprintf(buf + len, "%s", status);
	len += sprintf(buf + len, "<coopOrderSnap>%s</coopOrderSnap>", m_topup_info->tbOrderSnap.c_str());
	len += sprintf(buf + len, "<failedCode>%s</failedCode>", errCode);
	len += sprintf(buf + len, "<failedReason>%s</failedReason>", GlobalConfig::Instance()->errors[string(errCode)].c_str());
	len += sprintf(buf + len, "</response>");
	result = string(buf);
	return len;
}
//返回正确的信息
int TopupImpl::MakeSuccessReplay(const char* status, string &result){
	char buf[2048] = {0};
	int len = 0;
	len += sprintf(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	len += sprintf(buf + len, "<response>");
	len += sprintf(buf + len, "<tbOrderNo>%s</tbOrderNo>", m_topup_info->tbOrderNo.c_str());
	len += sprintf(buf + len, "<coopOrderNo>%lu</coopOrderNo>", m_topup_info->coopOrderNo);
	len += sprintf(buf + len, "%s", status);
	len += sprintf(buf + len, "<coopOrderSnap>%s</coopOrderSnap>", m_topup_info->tbOrderSnap.c_str());
	len += sprintf(buf + len, "<coopOrderSuccessTime>xxx</coopOrderSuccessTime>");
	len += sprintf(buf + len, "</response>");
	result = buf;
	return len;
}

int TopupImpl::CheckProduct(){
	printf("CheckProduct.....\n");

	ChargeBusiness *chargeBusiness = new ChargeBusiness();
	chargeBusiness->Init(m_conn);
	Product m_product;
	int ret =  chargeBusiness->GetTmallProduct(m_topup_info->cardId, m_product);
	if(ret == 0){
		m_topup_info->value = m_product.price;
		m_topup_info->op = m_product.op;
		m_topup_info->province = m_product.provinceId;
	}
	delete chargeBusiness;
	return ret;
}

int TopupImpl::SelectBestChannel(){
	printf("SelectBestChannel.....\n");
	ChargeBusiness *chargeBusiness = new ChargeBusiness();
	chargeBusiness->Init(m_conn);
	int channel_num = chargeBusiness->SelectBestChannel(m_topup_info->value, m_topup_info->province, m_topup_info->op,m_channels);	
	if(channel_num == 0){
		return 0;	
	}else if(channel_num == 1){
		return 1;	
	}else{
		sort(m_channels.begin(), m_channels.end(), ChannelRank);	
		return channel_num;
	}
	return 0;
}

int TopupImpl::CreateTmallOrder(){
	printf("CreateTmallOrder.....\n");
	ChargeBusiness *chargeBusiness = new ChargeBusiness();
	chargeBusiness->Init(m_conn);
	int ret = chargeBusiness->CreateTmallOrder(m_topup_info, m_channels[0]);
	delete chargeBusiness;
	return ret;
}

bool TopupImpl::CheckSign(){
	char md5[33] = {0};
    char signStr[2048] = {0};
    int len = 0;
    map<string, string>::iterator it = map_entitys.begin();
	string sign_str;
    for(;it != map_entitys.end(); ++it){
	    if(strcmp("sign", it->first.c_str()) == 0){
			sign_str = it->second;
			continue;
		}
		len += sprintf(signStr + len, "%s%s", it->first.c_str(), it->second.c_str());
	}
    len += sprintf(signStr + len, "%s", GlobalConfig::Instance()->private_key);
    str2md5(signStr,len, md5);
	if(sign_str.empty()){
		return false;
	}
	
	if(strcmp(sign_str.c_str(), md5) != 0){
		return false;
	}
	return true;	
}
