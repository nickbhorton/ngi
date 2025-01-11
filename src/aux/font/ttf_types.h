#pragma once

#include <cstdint>
#include <vector>

typedef uint32_t Fixed;
typedef int16_t FWord;
typedef int64_t LongDateTime;

#pragma pack(push, 1)
struct OffsetTable {
    uint32_t scalar_type;
    uint16_t num_tables;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;

    OffsetTable() = default;
    OffsetTable(const OffsetTable&) = delete;
    OffsetTable(OffsetTable&&) = default;
    OffsetTable& operator=(const OffsetTable&) = delete;
    OffsetTable& operator=(OffsetTable&&) = delete;
};

struct TableDirectoryEntry {
    char tag[4];
    uint32_t check_sum;
    uint32_t offset;
    uint32_t length;

    TableDirectoryEntry() = default;
    TableDirectoryEntry(const TableDirectoryEntry&) = delete;
    TableDirectoryEntry(TableDirectoryEntry&&) = delete;
    TableDirectoryEntry& operator=(const TableDirectoryEntry&) = delete;
    TableDirectoryEntry& operator=(TableDirectoryEntry&&) = default;
};

struct HeadTable {
    // unimplemented
    Fixed version;
    // unimplemented
    Fixed font_revision;

    uint32_t check_sum_adjustment;
    uint32_t magic_number;
    uint16_t flags;
    uint16_t units_per_em;
    int64_t created;
    int64_t modified;
    FWord x_min;
    FWord y_min;
    FWord x_max;
    FWord y_max;
    uint16_t mac_style;
    uint16_t lowest_rec_ppem;
    int16_t font_direction_hint;
    int16_t index_to_loc_format;
    int16_t glyph_data_format;

    HeadTable() = default;
    HeadTable(const HeadTable&) = delete;
    HeadTable(HeadTable&&) = default;
    HeadTable& operator=(const HeadTable&) = delete;
    HeadTable& operator=(HeadTable&&) = delete;
};

struct CmapSubtable {
    uint16_t platform_id;
    uint16_t platform_specific_id;
    uint32_t offset;

    CmapSubtable() = default;
    CmapSubtable(const CmapSubtable&) = delete;
    CmapSubtable(CmapSubtable&&) = default;
    CmapSubtable& operator=(const CmapSubtable&) = delete;
    CmapSubtable& operator=(CmapSubtable&&) = delete;
};

struct CmapSubtableFormat4 {
    uint16_t length;
    uint16_t language;
    uint16_t seg_count_x2;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
    std::vector<uint16_t> end_code;
    uint16_t reserved_pad;
    std::vector<uint16_t> start_code;
    std::vector<uint16_t> id_delta;
    std::vector<uint16_t> id_range_offset;
    std::vector<uint16_t> glyph_index_array;

    uint32_t get_glyph_index(uint16_t unicode_value);
    CmapSubtableFormat4() = default;
    CmapSubtableFormat4(const CmapSubtableFormat4&) = delete;
    CmapSubtableFormat4(CmapSubtableFormat4&&) = default;
    CmapSubtableFormat4& operator=(const CmapSubtableFormat4&) = delete;
    CmapSubtableFormat4& operator=(CmapSubtableFormat4&&) = delete;
};

struct CmapTable {
    uint16_t version;
    uint16_t number_subtables;
    std::vector<CmapSubtable> subtables;

    CmapTable() = default;
    CmapTable(const CmapTable&) = delete;
    CmapTable(CmapTable&&) = default;
    CmapTable& operator=(const CmapTable&) = delete;
    CmapTable& operator=(CmapTable&&) = delete;
};

struct GlyphCommon {
    int16_t number_of_contours;
    FWord x_min;
    FWord y_min;
    FWord x_max;
    FWord y_max;

    GlyphCommon() = default;
    GlyphCommon(const GlyphCommon&) = delete;
    GlyphCommon(GlyphCommon&&) = default;
    GlyphCommon& operator=(const GlyphCommon&) = delete;
    GlyphCommon& operator=(GlyphCommon&&) = default;
};

struct SimpleGlyph {
    GlyphCommon gc;
    std::vector<uint16_t> end_points_of_contours;
    uint16_t instruction_length;
    std::vector<uint8_t> instructions;
    std::vector<uint8_t> flags;
    std::vector<int16_t> x_coords;
    std::vector<int16_t> y_coords;

    SimpleGlyph() = default;
    SimpleGlyph(const SimpleGlyph&) = delete;
    SimpleGlyph(SimpleGlyph&&) = default;
    SimpleGlyph& operator=(const SimpleGlyph&) = delete;
    SimpleGlyph& operator=(SimpleGlyph&&) = delete;
};

struct MaxpTable {
    Fixed version;
    uint16_t number_glyphs;
    uint16_t max_points;
    uint16_t max_contours;
    uint16_t max_component_points;
    uint16_t max_component_contours;
    uint16_t max_zones;
    uint16_t max_twilight_points;
    uint16_t max_storage;
    uint16_t max_function_defs;
    uint16_t max_instruction_defs;
    uint16_t max_stack_elements;
    uint16_t max_size_of_instructions;
    uint16_t max_component_elements;
    uint16_t max_component_depth;

    MaxpTable() = default;
    MaxpTable(const MaxpTable&) = delete;
    MaxpTable(MaxpTable&&) = default;
    MaxpTable& operator=(const MaxpTable&) = delete;
    MaxpTable& operator=(MaxpTable&&) = delete;
};

struct LocaTable {
    std::vector<uint32_t> offsets;
};
#pragma pack(pop)
