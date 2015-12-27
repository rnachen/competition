/// @file       comm_key_value_config.h
/// @date       2013/12/05 21:03
///
/// @author
///
/// @brief      这个类用于管理键值对类型的配置文件

#ifndef COMM_KEY_VALUE_CONFIG_H_
#define COMM_KEY_VALUE_CONFIG_H_

#include <map>
#include <string>

class CommKeyValueConfig
{
public:
    CommKeyValueConfig();
    virtual ~CommKeyValueConfig();

    enum
    {
        EKP_OK = 0,
        EKP_INVALID_PARAM = -1, // 参数有误
        EKP_VALUE_BUF_TOO_SHORT = -2, // value的buf不够
    };

    enum IntValueType
    {
        EVALUE_INT10 = 0, // 配置中数据是以10进制表示
        EVALUE_INT16 = 1, // 配置中数据是以16进制表示
    };

    ///
    /// @fn
    /// @brief  从配置文件加载数据
    ///
    /// @param  conf_file_path [in] 要加载的配置文件路径
    ///
    /// @return 0 加载成功 ！0 加载失败，有可能是文件不存在，或没有读取权限
    ///
    int load_from_file(const char *conf_file_path);

    ///
    /// @fn
    /// @brief  获取加载的字符串类型的配置项值
    ///
    /// @param  config_name [in] 要获取的配置项名字
    /// @param  value [out] 存放配置项值
    /// @param  value_len [in] value的空间长度
    /// @param  default_value [in] 如果配置项不存在，则使用此默认值
    ///
    /// @return 0成功，！0参数错误
    ///
    int get_str(const char *config_name, char *value,
                unsigned int value_len, const char *default_value = NULL);

    ///
    /// @fn
    /// @brief  获取加载的整数类型的配置项值
    ///
    /// @param  config_name [in] 要获取的配置项名字
    /// @param  value [out] 存放配置项值
    /// @param  type [in] 数据在配置中的进制
    /// @param  default_value [in] 如果配置项不存在，则使用此默认值
    ///
    /// @return 0成功，！0参数错误
    ///
    int get_int(const char *config_name, int *value, int default_value = 0, IntValueType type = EVALUE_INT10);

    ///
    /// @fn
    /// @brief  获取加载的short类型的配置项值
    ///
    /// @param  config_name [in] 要获取的配置项名字
    /// @param  value [out] 存放配置项值
    /// @param  type [in] 数据在配置中的进制
    /// @param  default_value [in] 如果配置项不存在，则使用此默认值
    ///
    /// @return 0成功，！0参数错误
    ///
    int get_short(const char *config_name, short *value, short default_value = 0, IntValueType type = EVALUE_INT10);

    ///
    /// @fn
    /// @brief  获取加载的bool类型的配置项值
    ///
    /// @param  config_name [in] 要获取的配置项名字
    /// @param  value [out] 存放配置项值
    /// @param  default_value [in] 如果配置项不存在，则使用此默认值
    ///
    /// @return 0成功，！0参数错误
    ///
    int get_bool(const char *config_name, bool *value, bool default_value = false);

private:

    // 添加一个元素
    void add_item(const std::string &name, const std::string &value);

    // 得到对应key的值
    const std::string *get_item_value(const std::string &name) const;

private :
    std::map<std::string, std::string> values_;

}; // end of KeyValueConfig

#endif // COMM_KEY_VALUE_CONFIG_H_

