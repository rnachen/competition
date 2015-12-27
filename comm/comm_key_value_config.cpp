#include "comm_key_value_config.h"
#include "comm_string_util.h"
#include <fstream>
#include <vector>

using namespace std;

CommKeyValueConfig::CommKeyValueConfig()
{
}

CommKeyValueConfig::~CommKeyValueConfig()
{
}

void
CommKeyValueConfig::add_item(const std::string &name,
                         const std::string &value)
{
    values_[name] = value;
}

const string *
CommKeyValueConfig::get_item_value(const std::string &name) const
{
    map<string, string>::const_iterator it = values_.find(name);

    if (it != values_.end())
    {
        return &it->second;
    }

    // 未找到相应的项
    return NULL;
}

int
CommKeyValueConfig::load_from_file(const char *conf_file_path)
{
    values_.clear();

    ifstream fin(conf_file_path);

    if (!fin)
    {
        // 打开文件出错
        return EKP_INVALID_PARAM;
    }

    string line;
    vector<string> vec;

    while (getline(fin, line))
    {
        trim(line);
        trim(line, "\r");

        if (line.empty()
            || line[0] == '#')
        {
            // 空行或者注释行，忽略
            continue;
        }

        vec.clear();
        split(line.c_str(), vec);

        if (vec.size() == 2)
        {
            trim(vec[0]);
            trim(vec[1]);

            if (vec[0].length() != 0 && vec[1].length() != 0)
            {
                add_item(vec[0], vec[1]);
            }
        }
        else
        {
            // bad config line
            printf("bad config line in file %s. line=%s\n",
                   conf_file_path, line.c_str());
        }

    }

    return EKP_OK;
}

int
CommKeyValueConfig::get_str(const char *config_name,
                        char *value,
                        unsigned int value_len,
                        const char *default_value /* = NULL */)
{
    if (config_name == NULL
        || value == NULL
        || value_len == 0)
    {
        // 参数错误
        return EKP_INVALID_PARAM;
    }

    // 为保持与原来的key value类一致，需先将传入的buffer清0
    memset(value, 0, value_len);

    const string *item_value = get_item_value(config_name);

    if (item_value != NULL)
    {
        if (item_value->length() < value_len)
        {
            memcpy(value, item_value->c_str(), item_value->length());
            value[item_value->length()] = 0;
        }
        else
        {
            // 存储区长度不够
            return EKP_VALUE_BUF_TOO_SHORT;
        }
    }
    else
    {
        // 项不存在，使用默认值
        if (default_value != NULL)
        {
            size_t len = strlen(default_value);

            if (len >= value_len)
            {
                len = value_len - 1;
            }

            memcpy(value, default_value, len);
            value[len] = 0;
        }
    }

    return EKP_OK;
}

int
CommKeyValueConfig::get_int(const char *config_name,
                        int *value,
                        int default_value /* = 0 */,
                        CommKeyValueConfig::IntValueType type)
{
    assert(config_name != NULL && value != NULL);

    if (config_name == NULL
        || value == NULL)
    {
        // 参数错误
        return EKP_INVALID_PARAM;
    }

    const string *item_value = get_item_value(string(config_name));

    if (item_value != NULL)
    {
        if (type == EVALUE_INT10)
        {
            sscanf(item_value->c_str(), "%d", value);
        }
        else if (type == EVALUE_INT16)
        {
            sscanf(item_value->c_str(), "%x", value);
        }
        else
        {
            // 不支持的数据进制
            *value = default_value;
        }
    }
    else
    {
        *value = default_value;
    }

    return EKP_OK;
}

int
CommKeyValueConfig::get_short(const char *config_name,
                          short *value,
                          short default_value /* = 0 */,
                          CommKeyValueConfig::IntValueType type)
{
    int item_value;
    int ret = get_int(config_name, &item_value, default_value, type);

    if (ret != EKP_OK)
    {
        return ret;
    }

    *value = (short)item_value; // 超出short范围则丢失了，不处理

    return EKP_OK;
}

int
CommKeyValueConfig::get_bool(const char *config_name,
                         bool *value,
                         bool default_value /* = false */)
{
    int item_value;
    int ret = get_int(config_name, &item_value, (default_value ? 1 : 0), EVALUE_INT10);

    if (ret != EKP_OK)
    {
        return ret;
    }

    *value = (item_value != 0);
    return EKP_OK;
}
