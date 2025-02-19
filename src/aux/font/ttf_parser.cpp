#include "bezier.h"
#include "ttf_types.h"

#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <arpa/inet.h>

// #include "arrayalgebra.h"

namespace ngi::font
{

template <typename T> T read_type(std::ifstream& stream)
{
    T a{};
    stream.read(reinterpret_cast<char*>(&a), sizeof(a));
    if (sizeof(a) == 2) {
        a = ntohs(a);
    } else if (sizeof(a) == 4) {
        a = ntohl(a);
    }
    return a;
}

OffsetTable read_offset_table(std::ifstream& font_file)
{
    font_file.seekg(0, std::ios::beg);
    OffsetTable offset_table{};
    offset_table.scalar_type = read_type<uint32_t>(font_file);
    offset_table.num_tables = read_type<uint16_t>(font_file);
    offset_table.search_range = read_type<uint16_t>(font_file);
    offset_table.entry_selector = read_type<uint16_t>(font_file);
    offset_table.range_shift = read_type<uint16_t>(font_file);
    return offset_table;
};

std::map<std::string, TableDirectoryEntry>
read_table_directory(std::ifstream& font_file, OffsetTable const& offset_table)
{
    font_file.seekg(sizeof(OffsetTable), std::ios::beg);
    std::map<std::string, TableDirectoryEntry> table_directory{};
    for (size_t i = 0; i < offset_table.num_tables; i++) {
        TableDirectoryEntry tde{};
        for (size_t j = 0; j < 4; j++) {
            tde.tag[j] = read_type<char>(font_file);
        }
        tde.check_sum = read_type<uint32_t>(font_file);
        tde.offset = read_type<uint32_t>(font_file);
        tde.length = read_type<uint32_t>(font_file);
        table_directory[std::string(tde.tag, 4)] = std::move(tde);
    }
    return table_directory;
}

HeadTable read_head_table(
    std::ifstream& font_file,
    std::map<std::string, TableDirectoryEntry> const& table_directory
)
{
    HeadTable head_table{};
    font_file.seekg(table_directory.at("head").offset, std::ios::beg);
    head_table.version = read_type<Fixed>(font_file);
    head_table.font_revision = read_type<Fixed>(font_file);
    head_table.check_sum_adjustment = read_type<uint32_t>(font_file);
    // 1594834165 0x5F0F3CF5
    head_table.magic_number = read_type<uint32_t>(font_file);
    if (head_table.magic_number != 1594834165) {
        std::cerr << "magic number for head table is off\n";
    }
    head_table.flags = read_type<uint16_t>(font_file);
    head_table.units_per_em = read_type<uint16_t>(font_file);
    head_table.created = read_type<int64_t>(font_file);
    head_table.modified = read_type<int64_t>(font_file);
    head_table.x_min = read_type<FWord>(font_file);
    head_table.y_min = read_type<FWord>(font_file);
    head_table.x_max = read_type<FWord>(font_file);
    head_table.y_max = read_type<FWord>(font_file);
    head_table.mac_style = read_type<uint16_t>(font_file);
    head_table.lowest_rec_ppem = read_type<uint16_t>(font_file);
    head_table.font_direction_hint = read_type<int16_t>(font_file);
    head_table.index_to_loc_format = read_type<int16_t>(font_file);
    head_table.glyph_data_format = read_type<int16_t>(font_file);
    return head_table;
};

CmapTable read_cmap_table(
    std::ifstream& font_file,
    std::map<std::string, TableDirectoryEntry> const& table_directory
)
{
    CmapTable cmap_table{};
    std::vector<CmapSubtableFormat4> cmap_format4s{};
    font_file.seekg(table_directory.at("cmap").offset, std::ios::beg);
    cmap_table.version = read_type<uint16_t>(font_file);
    cmap_table.number_subtables = read_type<uint16_t>(font_file);
    for (auto k = 0; k < cmap_table.number_subtables; k++) {
        CmapSubtable subtable{};
        subtable.platform_id = read_type<uint16_t>(font_file);
        subtable.platform_specific_id = read_type<uint16_t>(font_file);
        subtable.offset = read_type<uint32_t>(font_file);
        cmap_table.subtables.push_back(std::move(subtable));
    }
    return cmap_table;
}

std::optional<CmapSubtableFormat4> read_cmap_format4(
    std::ifstream& font_file,
    std::map<std::string, TableDirectoryEntry> const& table_directory,
    CmapTable const& cmap_table,
    uint16_t platform_id
)
{
    for (auto const& subtable : cmap_table.subtables) {
        font_file.seekg(
            table_directory.at("cmap").offset + subtable.offset,
            std::ios::beg
        );
        uint16_t subtable_format{read_type<uint16_t>(font_file)};
        if (subtable_format == 4 && subtable.platform_id == platform_id) {
            CmapSubtableFormat4 cmap_format4{};
            cmap_format4.length = read_type<uint16_t>(font_file);
            cmap_format4.language = read_type<uint16_t>(font_file);
            cmap_format4.seg_count_x2 = read_type<uint16_t>(font_file);
            cmap_format4.search_range = read_type<uint16_t>(font_file);
            cmap_format4.entry_selector = read_type<uint16_t>(font_file);
            cmap_format4.range_shift = read_type<uint16_t>(font_file);
            for (auto k = 0; k < cmap_format4.seg_count_x2 / 2; k++) {
                cmap_format4.end_code.push_back(read_type<uint16_t>(font_file));
            }
            cmap_format4.reserved_pad = read_type<uint16_t>(font_file);
            if (cmap_format4.reserved_pad != 0) {
                std::cerr << "cmap subtable format 4 reserved padding "
                             "is not 0\n";
            }
            for (auto k = 0; k < cmap_format4.seg_count_x2 / 2; k++) {
                cmap_format4.start_code.push_back(read_type<uint16_t>(font_file)
                );
            }
            for (auto k = 0; k < cmap_format4.seg_count_x2 / 2; k++) {
                cmap_format4.id_delta.push_back(read_type<uint16_t>(font_file));
            }
            for (auto k = 0; k < cmap_format4.seg_count_x2 / 2; k++) {
                cmap_format4.id_range_offset.push_back(
                    read_type<uint16_t>(font_file)
                );
            }
            size_t size_of_subtable{};
            size_of_subtable += sizeof(cmap_format4.length);
            size_of_subtable += sizeof(cmap_format4.language);
            size_of_subtable += sizeof(cmap_format4.seg_count_x2);
            size_of_subtable += sizeof(cmap_format4.search_range);
            size_of_subtable += sizeof(cmap_format4.entry_selector);
            size_of_subtable += sizeof(cmap_format4.range_shift);
            size_of_subtable += sizeof(uint16_t) * cmap_format4.end_code.size();
            size_of_subtable += sizeof(cmap_format4.reserved_pad);
            size_of_subtable +=
                sizeof(uint16_t) * cmap_format4.start_code.size();
            size_of_subtable += sizeof(uint16_t) * cmap_format4.id_delta.size();
            size_of_subtable +=
                sizeof(uint16_t) * cmap_format4.id_range_offset.size();
            size_t size_diff{cmap_format4.length - size_of_subtable};
            for (auto k = 0; k < size_diff / 2; k++) {
                cmap_format4.glyph_index_array.push_back(
                    read_type<uint16_t>(font_file)
                );
            }
            return cmap_format4;
        }
    }
    return {};
}

MaxpTable read_maxp_table(
    std::ifstream& font_file,
    std::map<std::string, TableDirectoryEntry> const& table_directory
)
{
    // MAXP
    MaxpTable maxp_table{};
    font_file.seekg(table_directory.at("maxp").offset, std::ios::beg);
    maxp_table.version = read_type<Fixed>(font_file);
    maxp_table.number_glyphs = read_type<uint16_t>(font_file);
    maxp_table.max_points = read_type<uint16_t>(font_file);
    maxp_table.max_contours = read_type<uint16_t>(font_file);
    maxp_table.max_component_points = read_type<uint16_t>(font_file);
    maxp_table.max_component_contours = read_type<uint16_t>(font_file);
    maxp_table.max_zones = read_type<uint16_t>(font_file);
    maxp_table.max_twilight_points = read_type<uint16_t>(font_file);
    maxp_table.max_storage = read_type<uint16_t>(font_file);
    maxp_table.max_function_defs = read_type<uint16_t>(font_file);
    maxp_table.max_instruction_defs = read_type<uint16_t>(font_file);
    maxp_table.max_stack_elements = read_type<uint16_t>(font_file);
    maxp_table.max_size_of_instructions = read_type<uint16_t>(font_file);
    maxp_table.max_component_elements = read_type<uint16_t>(font_file);
    maxp_table.max_component_depth = read_type<uint16_t>(font_file);
    return maxp_table;
}

LocaTable read_loca_table(
    std::ifstream& font_file,
    std::map<std::string, TableDirectoryEntry> const& table_directory,
    HeadTable const& head_table,
    MaxpTable const& maxp_table
)
{
    LocaTable loca_table{};
    font_file.seekg(table_directory.at("loca").offset, std::ios::beg);
    for (size_t k = 0; k <= maxp_table.number_glyphs; k++) {
        if (head_table.index_to_loc_format) {
            auto offset{read_type<uint32_t>(font_file)};
            loca_table.offsets.push_back(offset);
        } else {
            loca_table.offsets.push_back(
                2 * static_cast<uint32_t>(read_type<uint16_t>(font_file))
            );
        }
    }
    return loca_table;
}

void read_coords(std::ifstream& font_file, SimpleGlyph& sg, bool is_x)
{
    uint8_t SHORT{static_cast<uint8_t>((is_x ? 0b0000'0010 : 0b0000'0100))};
    uint8_t SAME{static_cast<uint8_t>((is_x ? 0b0001'0000 : 0b0010'0000))};
    int16_t coord{0};
    for (size_t n = 0; n < sg.flags.size(); n++) {
        if (sg.flags[n] & SHORT) {
            int16_t diff = static_cast<int16_t>(read_type<uint8_t>(font_file));
            coord += (sg.flags[n] & SAME) ? diff : -diff;
        } else if (!(sg.flags[n] & SAME)) {
            int16_t diff = read_type<int16_t>(font_file);
            coord += diff;
        }
        if (is_x) {
            if (coord > sg.gc.x_max) {
                sg.gc.x_max = coord;
            } else if (coord < sg.gc.x_min) {
                sg.gc.x_min = coord;
            }
            sg.x_coords.push_back(coord);
        } else {
            if (coord > sg.gc.y_max) {
                sg.gc.y_max = coord;
            } else if (coord < sg.gc.y_min) {
                sg.gc.y_min = coord;
            }
            sg.y_coords.push_back(coord);
        }
    }
}

SimpleGlyph read_simple_glyph(
    std::ifstream& font_file,
    GlyphCommon&& gc,
    LocaTable const& loca_table,
    size_t index
)
{
    SimpleGlyph sg{};
    sg.gc = std::move(gc);
    size_t num_points{0};
    for (size_t n = 0; n < static_cast<size_t>(sg.gc.number_of_contours); n++) {
        uint16_t end_contour_index = read_type<uint16_t>(font_file);
        num_points =
            std::max(static_cast<size_t>(end_contour_index + 1), num_points);
        sg.end_points_of_contours.push_back(end_contour_index);
    }

    /*
    std::cout << " {";
    for (size_t n = 0; n < static_cast<size_t>(sg.gc.number_of_contours); n++) {
        std::cout << sg.end_points_of_contours[n]
                  << (n != static_cast<size_t>(sg.gc.number_of_contours) - 1
                          ? ", "
                          : "");
    }
    std::cout << "} " << "num_points: " << num_points << " ";
    */

    sg.instruction_length = read_type<uint16_t>(font_file);
    for (size_t n = 0; n < sg.instruction_length; n++) {
        sg.instructions.push_back(read_type<uint8_t>(font_file));
    }
    // std::cout << "\n";
    // std::cout << num_points << "\n";
    for (size_t n = 0; n < num_points; n++) {
        uint8_t flag = read_type<uint8_t>(font_file);
        sg.flags.push_back(flag);
        if (sg.flags[n] & 0b1100'0000) {
            std::cerr << "glyph flags have bits set when they "
                         "are supposed to be zero\n";
        }
        uint8_t constexpr REPEAT{0b0000'1000};
        if (sg.flags[n] & REPEAT) {
            uint8_t repeat_count = read_type<uint8_t>(font_file);
            for (size_t r = 0; r < repeat_count; r++) {
                sg.flags.push_back(flag);
                n++;
            }
        }
    }
    read_coords(font_file, sg, true);
    read_coords(font_file, sg, false);
    return sg;
}

std::variant<SimpleGlyph, int> read_glyph(
    std::ifstream& font_file,
    std::map<std::string, TableDirectoryEntry> const& table_directory,
    LocaTable const& loca_table,
    size_t index
)
{
    font_file.seekg(
        table_directory.at("glyf").offset + loca_table.offsets[index],
        std::ios::beg
    );
    GlyphCommon gc{};
    gc.number_of_contours = read_type<int16_t>(font_file);
    gc.x_min = read_type<FWord>(font_file);
    gc.y_min = read_type<FWord>(font_file);
    gc.x_max = read_type<FWord>(font_file);
    gc.y_max = read_type<FWord>(font_file);
    if (gc.number_of_contours > 0) {
        return read_simple_glyph(font_file, std::move(gc), loca_table, index);
    } else {
        // std::cout << " multi-glyph not implemented\n";
        return 0;
    }
    return 0;
}

std::vector<Contour> get_contours(SimpleGlyph const& g)
{
    assert(g.flags.size() == g.x_coords.size());
    assert(g.flags.size() == g.y_coords.size());
    std::vector<Contour> result{};
    size_t e = 0;
    Contour contour{};
    for (auto i = 0; i < g.flags.size(); i++) {
        contour.push_back(
            {{g.x_coords[i], g.y_coords[i]}, ((g.flags[i] & 1) > 0)}
        );
        if (g.end_points_of_contours[e] == i) {
            result.push_back(contour);
            contour.clear();
            e++;
        }
    }
    for (auto& c : result) {
        Contour new_contour{};
        for (size_t i = 0; i < c.size() - 1; i++) {
            auto fst = c[i];
            auto snd = c[i + 1];
            new_contour.push_back(fst);
            if (std::get<1>(fst) == std::get<1>(snd)) {
                int16_t x = (std::get<0>(fst)[0] + std::get<0>(snd)[0]) / 2;
                int16_t y = (std::get<0>(fst)[1] + std::get<0>(snd)[1]) / 2;
                new_contour.push_back({{x, y}, !std::get<1>(fst)});
            }
            if (i == c.size() - 2) {
                new_contour.push_back(snd);
                // if on curve
                if (std::get<1>(snd)) {
                    // add middle curve point at end
                    int16_t x =
                        (std::get<0>(snd)[0] + std::get<0>(c[0])[0]) / 2;
                    int16_t y =
                        (std::get<0>(snd)[1] + std::get<0>(c[0])[1]) / 2;
                    new_contour.push_back({{x, y}, false});
                }
            }
        }
        c = new_contour;
    }
    return result;
}

std::vector<SimpleGlyph> get_simple_glyphs(std::string const& font_filename)
{
    std::ifstream font_file{font_filename};

    if (!font_file.is_open() || font_file.bad() || font_file.fail()) {
        std::cerr << font_filename << " did not open\n";
        throw 3;
    }

    OffsetTable offset_table{read_offset_table(font_file)};
    auto table_directory{read_table_directory(font_file, offset_table)};
    MaxpTable maxp_table{read_maxp_table(font_file, table_directory)};
    HeadTable head_table{read_head_table(font_file, table_directory)};
    LocaTable loca_table{
        read_loca_table(font_file, table_directory, head_table, maxp_table)
    };
    CmapTable cmap_table{read_cmap_table(font_file, table_directory)};
    auto cmap_format4_opt{
        read_cmap_format4(font_file, table_directory, cmap_table, 0)
    };

    std::vector<SimpleGlyph> simple_glyphs{};
    for (auto k = 0; k < loca_table.offsets.size() - 1; k++) {
        auto glyph{read_glyph(font_file, table_directory, loca_table, k)};
        if (std::holds_alternative<SimpleGlyph>(glyph)) {
            simple_glyphs.push_back(std::move(std::get<SimpleGlyph>(glyph)));
        }
    }
    return simple_glyphs;
}

uint32_t CmapSubtableFormat4::get_glyph_index(uint16_t unicode_value)
{
    if (unicode_value == 0xFFFF) {
        return 0;
    }
    for (size_t i = 0; i < end_code.size(); i++) {
        if (end_code[i] >= unicode_value) {
            if (start_code[i] <= unicode_value) {
                std::cout << "i: " << i << "\n";
                std::cout << "start_code: " << start_code[i] << "\n";
                std::cout << "end_code: " << end_code[i] << "\n";
                std::cout << "id_delta: " << id_delta[i] << "\n";
                std::cout << "id_range_offset / 2: " << id_range_offset[i] / 2
                          << "\n";
                uint32_t glyph_index =
                    (id_range_offset[i] / 2 - id_range_offset.size()) +
                    (unicode_value - start_code[i]);
                std::cout << "glyph_index_array_index: " << glyph_index << "\n";
                std::cout << "glyph index: " << glyph_index_array[glyph_index]
                          << "\n";
                return glyph_index_array[glyph_index];
            } else {
                return 0;
            }
        }
    }
    return 0;
}
} // namespace ngi::font
