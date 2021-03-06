/*************************************************************************
    > File Name: TopupZkcl.cpp
    > Author: desionwang
    > Mail: wdxin1322@qq.com 
    > Created Time: Sat 08 Feb 2014 04:16:03 PM CST
 ************************************************************************/

#include "TopupZkcl.h"
#include "TopupUtils.h"
#include "HttpClient.h"
#include "GlobalConfig.h"
#include "selog.h"
using namespace std;
using namespace  ::topupinterface;
using namespace boost;

extern LOG_HANDLE g_logHandle;

TopupZkcl::TopupZkcl(){
}

TopupZkcl::~TopupZkcl(){
}

void TopupZkcl::Log()
{
}


/**初始化连接**/
int TopupZkcl::Init(TopupInfo* topup_info)
{
    m_topup_info = topup_info;
    m_topup_info->log_len = 0;
    m_topup_info->err_log_len = 0;
    m_conn = topup_info->conn;
    return 0;
}

//error code
#define LAKE_PARAM_ERR        "0101"
#define NO_PRODUCT_ERR        "0305"
#define PRODUCT_MAIN_ERR      "0304"
#define SIGN_NOMATCH_ERR      "0102"
#define REQUEST_FAILED_ERR    "0104"
#define SAME_ORDER_EERR       "0301"
#define INVALID_PRODUCT_ERR   "0302"

//处理FCGI的请求，根据请求的URI判断如何处理
int TopupZkcl::HandleRequest(const TopupRequest& request, string &result){
    TP_WRITE_LOG(m_topup_info, "#%d\t[%s]\t%s\t%s\t%d",m_topup_info->seqid, request.uri.c_str()
            ,request.query.c_str(), request.checksum.c_str(), request.itimestamp);
    const char *params = request.query.c_str();
    const char *uri = request.uri.c_str();
    uint32_t request_time = request.itimestamp;
    const char *md5str = request.checksum.c_str();
    const char *m_interface = NULL;
    //解析post参数
    parse_params(params, &map_entitys);
    //解析query参数，并封装结构
    parse_query(params, m_topup_info);
    if(uri != NULL){
        m_interface = strrchr(uri, '/');
        if(m_interface == NULL){
            return 1;
        }
    }
    //根据调用的URI参数判断调用的相应接口
    if(strcmp(m_interface + 1, "pay") == 0){
        //调用充值接口
        ZkclCharge(result);
    }
#ifdef DEBUG
    map<string, string>::iterator it = map_entitys.begin();
    for(;it != map_entitys.end(); ++it){
        printf("key:%s\tvalue:%s\n", it->first.c_str(), it->second.c_str());
    }
#endif
    return 0;
}

///charge up interface
int TopupZkcl::ZkclCharge(string &response){
    //TODO 验证参数的正确性
    if(m_topup_info->qs_info.coopId.empty()){
        MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) no coopId %s", LAKE_PARAM_ERR);
        return 1;
    }
    if(m_topup_info->qs_info.tbOrderNo.empty()){
        MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) no tbOrderNo %s", LAKE_PARAM_ERR);
        return 1;
    }
    if(m_topup_info->qs_info.customer.empty()){
        MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) no customer %s", LAKE_PARAM_ERR);
        return 1;
    }
    if(m_topup_info->qs_info.sum == 0.){
        MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) no sum %s", LAKE_PARAM_ERR);
        return 1;
    }
    if(m_topup_info->qs_info.sign.empty()){
        MakeErrReplay(LAKE_PARAM_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) no sign %s", LAKE_PARAM_ERR);
        return 1;
    }
    //验证加密
    if(!CheckSign()){
        MakeErrReplay(SIGN_NOMATCH_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) sign error %s", SIGN_NOMATCH_ERR);
        return 4;
    }
    //验证重复
    int ret = QueryOrder();
    if(ret > 0){
        MakeErrReplay(SAME_ORDER_EERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) same order error %s", SAME_ORDER_EERR);    
        return 5;
    }

    //TODO 选择正确的产品，所有产品信息加入缓存，商品更新发送通知，重新加载缓存
    int check_product = CheckProduct();
    TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) PRODUCT:%d o:%d p:%d",check_product,
                    m_topup_info->qs_info.op ,m_topup_info->qs_info.province);
    if(check_product == 2){
        MakeErrReplay(NO_PRODUCT_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) no such product error %s", NO_PRODUCT_ERR);    
        return 2;
    }
    //TODO 选择最优的渠道，渠道信息同样加入缓存，信息更新，重新加载
    int selectChannel = SelectBestChannel();
    if(selectChannel <= 0){
        MakeErrReplay(PRODUCT_MAIN_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) select channel err %s", PRODUCT_MAIN_ERR);    
        return 3;
    }
    TP_WRITE_LOG(m_topup_info, "\tCHANNEL:%d id:%d name:%s dis:%f pri:%d int:%s",selectChannel,
            m_topup_info->channels[0].channelId,m_topup_info->channels[0].sname.c_str(),m_topup_info->channels[0].discount,
            m_topup_info->channels[0].priority, m_topup_info->channels[0].interfaceName.c_str());
    //TODO 建立订单
    int create_status = CreateTmallOrder();
    if(create_status < 0){
        MakeErrReplay(PRODUCT_MAIN_ERR, SORDER_FAILED, response);
        TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) fail to create order %s\terr code:%d", PRODUCT_MAIN_ERR, create_status);
        return 5;    
    }
    //TODO 返回结果
    TP_WRITE_LOG(m_topup_info, "\t(ZkclCharge) success create order");
    MakeSuccessReplay(SUNDERWAY, response);
    
    //设置通知状态
    m_topup_info->notify = 0;
    return 0;
}


//返回错误信息
int TopupZkcl::MakeErrReplay(const char* errCode,const char* status, string &result){
    char buf[2048] = {0};
    int len = 0;
    len += sprintf(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    len += sprintf(buf + len, "<response>");
    len += sprintf(buf + len, "<tbOrderNo>%s</tbOrderNo>", m_topup_info->qs_info.tbOrderNo.c_str());
    len += sprintf(buf + len, "<coopOrderNo>%s</coopOrderNo>", m_topup_info->qs_info.coopOrderNo.c_str());
    len += sprintf(buf + len, "%s", status);
    len += sprintf(buf + len, "<failedCode>%s</failedCode>", errCode);
    len += sprintf(buf + len, "<failedReason>%s</failedReason>", GlobalConfig::Instance()->errors[string(errCode)].c_str());
    len += sprintf(buf + len, "</response>");
    result = string(buf);
    return len;
}
//返回正确的信息
int TopupZkcl::MakeSuccessReplay(const char* status, string &result){
    char buf[2048] = {0};
    int len = 0;
    len += sprintf(buf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    len += sprintf(buf + len, "<response>");
    len += sprintf(buf + len, "<tbOrderNo>%s</tbOrderNo>", m_topup_info->qs_info.tbOrderNo.c_str());
    len += sprintf(buf + len, "<coopOrderNo>%s</coopOrderNo>", m_topup_info->qs_info.coopOrderNo.c_str());
    len += sprintf(buf + len, "%s", status);
    string ts;
    get_time_now("%Y%m%d%H%M%S", ts);
    len += sprintf(buf + len, "<coopOrderSuccessTime>%s</coopOrderSuccessTime>", ts.c_str());
    len += sprintf(buf + len, "</response>");
    result = buf;
    return len;
}
// ret = 0 正常
// ret = 1 出错异常
// ret = 2 没有相应的产品
// ret = 3 购买数量不合法
int TopupZkcl::CheckProduct(){
    char phone[8] = {0};
    strncpy(phone, m_topup_info->qs_info.customer.c_str(), 7);
    map<string, uint8_t>::iterator iter;
    int t_op = -1;
    int t_province = -1;
    uint8_t info;
    pthread_rwlock_rdlock(&(GlobalConfig::Instance()->tsc_rwlock));
    iter = GlobalConfig::Instance()->tsc_map.find(phone);
    pthread_rwlock_unlock(&(GlobalConfig::Instance()->tsc_rwlock));
    if(iter != GlobalConfig::Instance()->tsc_map.end()){
        info = iter->second;
        t_op = info & 0x3;
        t_province = info >> 2;
     }else{
        int ret = parse_tsc(phone ,&t_op, &t_province, GlobalConfig::Instance()->province_map);
        if(ret != 0){
           return 2;
        }else{
            //add
            pthread_rwlock_wrlock(&(GlobalConfig::Instance()->tsc_rwlock));
            info = t_province;
            info <<= 2;
            info += t_op;
            GlobalConfig::Instance()->tsc_map.insert(make_pair(string(phone), info));
            pthread_rwlock_unlock(&(GlobalConfig::Instance()->tsc_rwlock));
        }
     }
    if(t_op != -1 && t_province != -1){
        m_topup_info->qs_info.op = t_op;
        m_topup_info->qs_info.province = t_province;
    }
    return 0;
}

int TopupZkcl::SelectBestChannel(){
    ChargeBusiness *chargeBusiness = new ChargeBusiness();
    chargeBusiness->Init(m_conn);
    int channel_num = chargeBusiness->SelectBestChannel(m_topup_info->qs_info.value, m_topup_info->qs_info.province,
               m_topup_info->qs_info.op,m_topup_info->channels);    
#ifdef DEBUG
    printf("SelectBestChannel.....num:%d\n", channel_num);
#endif
    if(chargeBusiness->HasError()){
        vector<string> errors = chargeBusiness->GetErrors();
        write_err_msg(m_topup_info, errors);
    }
    delete chargeBusiness;
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

// ret = 0 正常
// ret = -1 异常
// ret = -2 生成sysno异常
// ret = -3 生成时间异常
int TopupZkcl::CreateTmallOrder(){
    ChargeBusiness *chargeBusiness = new ChargeBusiness();
    chargeBusiness->Init(m_conn);
    //also put the topup info to redis
    int ret = chargeBusiness->CreateTmallOrder(m_topup_info, m_topup_info->channels[0]);
    if(ret != 0){
        TP_WRITE_ERR(m_topup_info, "#%d [CreateTmallOrder] ChargeBusiness CreateTmallOrder fail:%d\n",
                m_topup_info->seqid, ret);    
        if(chargeBusiness->HasError()){
             vector<string> errors = chargeBusiness->GetErrors();
             write_err_msg(m_topup_info, errors);                                                                                           
        }
    }
    delete chargeBusiness;
    return ret;
}

int TopupZkcl::QueryOrder(){
    RedisClient *redis = new RedisClient();
    bool redis_status = false;
    int ret = 0;
    if(redis->connect(GlobalConfig::Instance()->s_redis_ip, GlobalConfig::Instance()->n_redis_port)){
        string topup_data;
        redis->select(1);
        char buf[256] = {0};
        sprintf(buf, "%s_%d", m_topup_info->qs_info.customer.c_str(), m_topup_info->qs_info.value);
        redis_status = redis->get(buf, topup_data);
        if(redis_status){
            delete redis;
            return 1;
        }
        redis_status = redis->get(m_topup_info->qs_info.tbOrderNo, topup_data);
        if(redis_status){
            //解析json    
            Json::Reader reader;
            Json::Value root;
            reader.parse(topup_data, root);
            m_topup_info->status = (OrderStatus)root["status"].asInt();
            delete redis;
            return 2;
        }
    }
    ChargeBusiness *chargeBusiness = new ChargeBusiness();
    chargeBusiness->Init(m_conn);
    ret = chargeBusiness->QueryOrder(m_topup_info);
    delete chargeBusiness;
    delete redis;
    return ret;
}

bool TopupImpl::CheckSign(){

/***
 * 验证签名的正确性
 */
bool TopupZkcl::CheckSign(){
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

int TopupZkcl::UpdateStatus(){
    ChargeBusiness *chargeBusiness = new ChargeBusiness();
    if(!chargeBusiness){
        return -1;
    }
    chargeBusiness->Init(m_conn);
    int ret = chargeBusiness->UpdateOrderStatus(m_topup_info);
    delete chargeBusiness;
    return ret;
}

/***
 * 内部接口不使用notify
 */
int TopupZkcl::Notify()
{
    return 0;
}

