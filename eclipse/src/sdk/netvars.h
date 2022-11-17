#pragma once
#include <cstdint>
#include <format>
#include <string>
#include <unordered_map>

#include "interface.h"

// snipped from uc, i liked this macro
#define NETVAR(var, type, name) type& var() { static auto offset = netvars::offsets[FNV(name)]; return *reinterpret_cast<type*>(std::uintptr_t(this) + offset); }
#define FNV(string) hash::fnv1<uint32_t>::hash(string)

// https://gist.github.com/filsinger/1255697/21762ea83a2d3c17561c8e6a29f44249a4626f9e
namespace hash
{
    template <typename S> struct fnv_internal;
    template <typename S> struct fnv1;
    template <typename S> struct fnv1a;

    template <> struct fnv_internal<uint32_t>
    {
        constexpr static uint32_t default_offset_basis = 0x811C9DC5;
        constexpr static uint32_t prime = 0x01000193;
    };

    template <> struct fnv1<uint32_t> : public fnv_internal<uint32_t>
    {
        constexpr static inline uint32_t hash(const char* aString, const uint32_t val = default_offset_basis)
        {
            return (aString[0] == '\0') ? val : hash(&aString[1], (val * prime) ^ uint32_t(aString[0]));
        }
    };

    template <> struct fnv1a<uint32_t> : public fnv_internal<uint32_t>
    {
        constexpr static inline uint32_t hash(const char* aString, const uint32_t val = default_offset_basis)
        {
            return (aString[0] == '\0') ? val : hash(&aString[1], (val ^ uint32_t(aString[0])) * prime);
        }
    };
} 


class RecvTable;
class RecvProp;

class ClientClass
{
public:
    void* m_pCreateFn;
    void* m_pCreateEventFn;
    char* m_pNetworkName;
    RecvTable* m_pRecvTable;
    ClientClass* m_pNext;
    int                m_ClassID;
};

class RecvTable
{
public:

    RecvProp* m_pProps;
    int            m_nProps;
    void* m_pDecoder;
    char* m_pNetTableName;
    bool        m_bInitialized;
    bool        m_bInMainList;
};

class RecvProp
{
public:
    char* m_pVarName;
    void* m_RecvType;
    int                     m_Flags;
    int                     m_StringBufferSize;
    int                     m_bInsideArray;
    const void* m_pExtraData;
    RecvProp* m_pArrayProp;
    void* m_ArrayLengthProxy;
    void* m_ProxyFn;
    void* m_DataTableProxyFn;
    RecvTable* m_pDataTable;
    int                     m_Offset;
    int                     m_ElementStride;
    int                     m_nElements;
    const char* m_pParentArrayPropName;
};

namespace netvars
{
    std::unordered_map<uint32_t, size_t> offsets{ };

    void dump();
    void get_offsets(RecvTable* table, const char* table_name);
}

void netvars::dump()
{
    static auto base_client = interfaces::get<IBaseClientDLL>("client.dll", "VClient018");
    auto client_class = reinterpret_cast<ClientClass*>(base_client->GetAllClasses());

    for (auto current = client_class;
        current;
        current = current->m_pNext)
    {
        if (!current || !current->m_pRecvTable)
            continue; // Continue if there is no table to loop through

        auto table = current->m_pRecvTable;
        netvars::get_offsets(table, table->m_pNetTableName);
    }
}

void netvars::get_offsets(RecvTable* table, const char* table_name)
{
    if (!table)
        return;

    // printf("[+] %s: %d\n", table_name, table->m_nProps);
    for (int i = 0; i < table->m_nProps; ++i)
    {
        auto prop = table->m_pProps[i];

        if (prop.m_pDataTable)
            netvars::get_offsets(prop.m_pDataTable, prop.m_pDataTable->m_pNetTableName);         
        else
        {
            auto formatted_string = std::format("{}.{}", table->m_pNetTableName, prop.m_pVarName);

            // Skip if we've already logged this netvar 
            if (netvars::offsets[FNV(formatted_string.c_str())])
                continue; 

            // Skip if it's a "numbered" netvar
            if (isdigit(prop.m_pVarName[0]) || !_stricmp(prop.m_pVarName, "000"))
                continue;

            netvars::offsets[FNV(formatted_string.c_str())] = prop.m_Offset;
        }
    }
}
