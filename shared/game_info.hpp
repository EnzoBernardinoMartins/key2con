#pragma once

#include <vector>
#include <string>
#include <switch.h>
#include <iostream>
#include <malloc.h>
#include <algorithm>
#include <unordered_set>

struct GameInfo
{
    std::string title;
    std::string title_id;
};

std::vector<std::string> get_application_titles(FILE* log, const std::vector<u64>& application_ids)
{
    std::vector<std::string> titles;

    if (application_ids.empty())
        return titles;

    AsyncValue async_value{};
    s32 count = static_cast<s32>(application_ids.size());

    size_t buffer_size = count * sizeof(NacpLanguageEntry) + count * sizeof(u64) + count * sizeof(NsApplicationControlData);
    buffer_size = (buffer_size + 0xFFF) & ~static_cast<size_t>(0xFFF);
    void* buffer = memalign(0x1000, buffer_size);

    if (buffer == NULL)
    {
        fprintf(log, "memalign failed: size=%zu\n", buffer_size);
        fflush(log);
        return titles;
    }

    Result rc = nsListApplicationTitle(&async_value, NsApplicationControlSource_Storage,
        application_ids.data(), count, buffer, buffer_size);

    fprintf(log, "nsListApplicationTitle: rc=0x%08X, count=%d\n",
        static_cast<unsigned int>(rc), count);
    fflush(log);

    if (R_FAILED(rc))
    {
        free(buffer);
        return titles;
    }

    s32 offset = -1;
    u64 result_size = 0;

    rc = asyncValueGet(&async_value, &offset, sizeof(offset));

    if (R_SUCCEEDED(rc))
        rc = asyncValueGetSize(&async_value, &result_size);

    fprintf(log, "async title result: rc=0x%08X, offset=%d, size=%llu\n",
        static_cast<unsigned int>(rc), offset, static_cast<unsigned long long>(result_size));
    fflush(log);

    if (R_SUCCEEDED(rc) &&
        offset >= 0 &&
        static_cast<size_t>(offset) <= buffer_size &&
        result_size >= static_cast<u64>(count) * sizeof(NacpLanguageEntry) &&
        result_size <= buffer_size - static_cast<size_t>(offset))
    {
        NacpLanguageEntry* entries = reinterpret_cast<NacpLanguageEntry*>(
            static_cast<u8*>(buffer) + offset);

        for (s32 entry = 0; entry < count; entry++)
        {
            titles.push_back(entries[entry].name);
        }
    }
    else
    {
        fprintf(log, "Invalid or failed application title data\n");
        fflush(log);
    }

    asyncValueClose(&async_value);
    free(buffer);

    return titles;
};

std::vector<GameInfo> get_recent_games_list(FILE* log)
{
    s32 total_entries = 0;
    s32 start_entry_index = 0;
    s32 end_entry_index = 0;
    s32 total_out = 0;

    Result rc = pdmqryGetAvailablePlayEventRange(&total_entries, &start_entry_index, &end_entry_index);

    fprintf(log, "PDM range: rc=0x%08X, total=%d, start=%d, end=%d\n",
        static_cast<unsigned int>(rc), total_entries, start_entry_index, end_entry_index);
    fflush(log);

    if (R_FAILED(rc) || total_entries <= 0)
        return {};

    std::vector<PdmAppletEvent> list(static_cast<size_t>(total_entries));

    rc = pdmqryQueryAppletEvent(start_entry_index, true, list.data(), total_entries, &total_out);

    fprintf(log, "PDM query: rc=0x%08X, total_out=%d\n",
        static_cast<unsigned int>(rc), total_out);
    fflush(log);

    if (R_FAILED(rc) || total_out <= 0)
        return {};

    if (total_out > total_entries)
        total_out = total_entries;

    std::sort(list.begin(), list.begin() + total_out,
        [](const PdmAppletEvent& a, const PdmAppletEvent& b)
        {
            return a.entry_index > b.entry_index;
        });

    constexpr size_t max_recent_games = 40;

    std::vector<u64> applications_ids;
    std::unordered_set<u64> seen_ids;

    applications_ids.reserve(max_recent_games);
    seen_ids.reserve(max_recent_games);

    for (s32 i = 0; i < total_out && applications_ids.size() < max_recent_games; i++)
    {
        u64 program_id = list[i].program_id;

        if (program_id == 0)
            continue;

        if (seen_ids.insert(program_id).second)
            applications_ids.push_back(program_id);
    }

    fprintf(log, "Unique recent application IDs: %zu\n", applications_ids.size());
    fflush(log);

    std::vector<std::string> titles = get_application_titles(log, applications_ids);

    std::vector<GameInfo> games;

    for (size_t i = 0; i < applications_ids.size() && i < titles.size(); i++)
    {
        if (titles[i].empty())
            continue;

        GameInfo game;
        char title_id[17];

        snprintf(title_id, sizeof(title_id), "%016lX",
            static_cast<unsigned long>(applications_ids[i]));

        game.title_id = title_id;
        game.title = titles[i];

        games.push_back(game);
    }

    fprintf(log, "Games loaded: %zu\n", games.size());
    fflush(log);

    return games;
};