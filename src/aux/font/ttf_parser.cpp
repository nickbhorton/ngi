#include "bezier.h"
#include "ttf_types.h"

#include <bitset>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <arpa/inet.h>

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

int main()
{
    std::string font_filename{"/usr/share/fonts/TTF/FiraCode-Regular.ttf"};
    std::ifstream font_file{font_filename};

    if (!font_file.is_open() || font_file.bad() || font_file.fail()) {
        std::cerr << font_filename << " did not open\n";
        return 1;
    }
    OffsetTable offset_table{};
    offset_table.scalar_type = read_type<uint32_t>(font_file);
    offset_table.num_tables = read_type<uint16_t>(font_file);
    offset_table.search_range = read_type<uint16_t>(font_file);
    offset_table.entry_selector = read_type<uint16_t>(font_file);
    offset_table.range_shift = read_type<uint16_t>(font_file);
    std::vector<TableDirectoryEntry> table_directory{offset_table.num_tables};
    for (size_t i = 0; i < offset_table.num_tables; i++) {
        for (size_t j = 0; j < 4; j++) {
            table_directory[i].tag[j] = read_type<char>(font_file);
        }
        table_directory[i].check_sum = read_type<uint32_t>(font_file);
        table_directory[i].offset = read_type<uint32_t>(font_file);
        table_directory[i].length = read_type<uint32_t>(font_file);

        /*
        std::cout << std::string(table_directory[i].tag, 4) << " "
                  << table_directory[i].length << "\n";
        */
    }

    // HEAD
    HeadTable head_table{};
    for (auto i = 0; i < table_directory.size(); i++) {
        if (std::string(table_directory[i].tag, 4) == "head") {
            // no idea why +4
            font_file.seekg(table_directory[i].offset + 4, std::ios::beg);
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

            /*
            std::cout << "x: " << head_table.x_max << " " << head_table.x_max
                      << "\n";
            std::cout << "y: " << head_table.y_max << " " << head_table.y_max
                      << "\n";
            */
        }
    }

    // CMAP
    CmapTable cmap_table{};
    std::vector<CmapSubtableFormat4> cmap_format4s{};
    for (auto i = 0; i < table_directory.size(); i++) {
        if (std::string(table_directory[i].tag, 4) == "cmap") {
            font_file.seekg(table_directory[i].offset, std::ios::beg);
            cmap_table.version = read_type<uint16_t>(font_file);
            cmap_table.number_subtables = read_type<uint16_t>(font_file);
            std::cout << "cmap subtables: " << cmap_table.number_subtables
                      << "\n";
            for (auto k = 0; k < cmap_table.number_subtables; k++) {
                CmapSubtable subtable{};
                subtable.platform_id = read_type<uint16_t>(font_file);
                subtable.platform_specific_id = read_type<uint16_t>(font_file);
                subtable.offset = read_type<uint32_t>(font_file);
                /*
                std::cout << subtable.platform_id << " "
                          << subtable.platform_specific_id << " "
                          << subtable.offset << "\n";
                          */
                cmap_table.subtables.push_back(subtable);
            }
            for (auto const& subtable : cmap_table.subtables) {
                font_file.seekg(
                    table_directory[i].offset + subtable.offset,
                    std::ios::beg
                );
                uint16_t subtable_format{read_type<uint16_t>(font_file)};
                /*
                std::cout << "subtable " << subtable.platform_id << ":"
                          << subtable.platform_specific_id
                          << " format: " << subtable_format << "\n";
                          */
                if (subtable_format == 4) {
                    CmapSubtableFormat4 cmap_format4{};
                    cmap_format4.length = read_type<uint16_t>(font_file);
                    cmap_format4.language = read_type<uint16_t>(font_file);
                    cmap_format4.seg_count_x2 = read_type<uint16_t>(font_file);
                    cmap_format4.search_range = read_type<uint16_t>(font_file);
                    cmap_format4.entry_selector =
                        read_type<uint16_t>(font_file);
                    cmap_format4.range_shift = read_type<uint16_t>(font_file);
                    for (auto k = 0; k < cmap_format4.seg_count_x2 / 2; k++) {
                        cmap_format4.end_code.push_back(
                            read_type<uint16_t>(font_file)
                        );
                    }
                    cmap_format4.reserved_pad = read_type<uint16_t>(font_file);
                    if (cmap_format4.reserved_pad != 0) {
                        std::cerr << "cmap subtable format 4 reserved padding "
                                     "is not 0\n";
                    }
                    for (auto k = 0; k < cmap_format4.seg_count_x2 / 2; k++) {
                        cmap_format4.start_code.push_back(
                            read_type<uint16_t>(font_file)
                        );
                    }
                    for (auto k = 0; k < cmap_format4.seg_count_x2 / 2; k++) {
                        cmap_format4.id_delta.push_back(
                            read_type<uint16_t>(font_file)
                        );
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
                    size_of_subtable +=
                        sizeof(uint16_t) * cmap_format4.end_code.size();
                    size_of_subtable += sizeof(cmap_format4.reserved_pad);
                    size_of_subtable +=
                        sizeof(uint16_t) * cmap_format4.start_code.size();
                    size_of_subtable +=
                        sizeof(uint16_t) * cmap_format4.id_delta.size();
                    size_of_subtable +=
                        sizeof(uint16_t) * cmap_format4.id_range_offset.size();
                    /*
                    std::cout << "cmap subtable format 4\n";
                    std::cout << "\tlength: " << cmap_format4.length << "\n";
                    std::cout << "\tseg count " << cmap_format4.seg_count_x2 / 2
                              << "\n";
                              */
                    size_t size_diff{cmap_format4.length - size_of_subtable};
                    for (auto k = 0; k < size_diff / 2; k++) {
                        cmap_format4.glyph_index_array.push_back(
                            read_type<uint16_t>(font_file)
                        );
                        // std::cout << cmap_format4.glyph_index_array[k] << "
                        // ";
                    }
                    /*
                    for (auto k = 0; k < cmap_format4.seg_count_x2 / 2; k++) {
                        std::cout
                            << "[" << cmap_format4.start_code[k] << ", "
                            << cmap_format4.end_code[k]
                            << "] delta:" << cmap_format4.id_delta[k]
                            << " offset:" << cmap_format4.id_range_offset[k]
                            << "\n";
                    }
                    // std::cout << "\n";
                    std::cout << "gliph_index_array size: "
                              << cmap_format4.glyph_index_array.size() << "\n";
                              */
                    cmap_format4s.push_back(cmap_format4);
                }
            }
        }
    }

    // MAXP
    MaxpTable maxp_table{};
    for (auto i = 0; i < table_directory.size(); i++) {
        if (std::string(table_directory[i].tag, 4) == "maxp") {
            font_file.seekg(table_directory[i].offset + 4, std::ios::beg);
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
            maxp_table.max_size_of_instructions =
                read_type<uint16_t>(font_file);
            maxp_table.max_component_elements = read_type<uint16_t>(font_file);
            maxp_table.max_component_depth = read_type<uint16_t>(font_file);
            std::cout << "number of glyphs: " << maxp_table.number_glyphs
                      << "\n";
            /*
            std::cout << "max number of points: " << maxp_table.max_points
                      << "\n";
                      */
        }
    }

    // LOCA
    LocaTable loca_table{};
    for (auto i = 0; i < table_directory.size(); i++) {
        if (std::string(table_directory[i].tag, 4) == "loca") {
            font_file.seekg(table_directory[i].offset, std::ios::beg);
            for (size_t k = 0; k < maxp_table.number_glyphs; k++) {
                if (head_table.index_to_loc_format) {
                    auto offset{read_type<uint32_t>(font_file)};
                    loca_table.offsets.push_back(offset);
                } else {
                    loca_table.offsets.push_back(read_type<uint16_t>(font_file)
                    );
                }
            }
        }
    }

    // GLYF
    for (auto i = 0; i < table_directory.size(); i++) {
        if (std::string(table_directory[i].tag, 4) == "glyf") {
            for (uint32_t k = 'A'; k < 'B'; k++) {
                std::cout << (char)k << ": ";
                GlyphCommon gc{};
                auto glyph_offset =
                    loca_table.offsets[cmap_format4s[0].get_glyph_index(k)];
                font_file.seekg(
                    table_directory[i].offset + glyph_offset,
                    std::ios::beg
                );
                gc.number_of_contours = read_type<int16_t>(font_file);
                gc.x_min = read_type<FWord>(font_file);
                gc.y_min = read_type<FWord>(font_file);
                gc.x_max = read_type<FWord>(font_file);
                gc.y_max = read_type<FWord>(font_file);
                std::cout << glyph_offset << "  " << gc.number_of_contours
                          << " [" << gc.x_min << ", " << gc.x_max << "] ["
                          << gc.y_min << ", " << gc.y_max << "]";
                if (gc.number_of_contours > 0) {
                    SimpleGlyph sg{};
                    sg.gc = gc;
                    for (size_t n = 0;
                         n < static_cast<size_t>(sg.gc.number_of_contours);
                         n++) {
                        sg.end_points_of_contours.push_back(
                            read_type<uint16_t>(font_file)
                        );
                    }

                    std::cout << " {";
                    for (size_t n = 0;
                         n < static_cast<size_t>(sg.gc.number_of_contours);
                         n++) {
                        std::cout << sg.end_points_of_contours[n]
                                  << (n != static_cast<size_t>(
                                               sg.gc.number_of_contours
                                           ) - 1
                                          ? ", "
                                          : "");
                    }
                    std::cout << "} ";

                    sg.instruction_length = read_type<uint16_t>(font_file);
                    std::cout << sg.instruction_length;
                    for (size_t n = 0; n < sg.instruction_length; n++) {
                        sg.instructions.push_back(read_type<uint8_t>(font_file)
                        );
                    }
                    std::cout << "\n";
                    for (size_t n = 0;
                         n < sg.end_points_of_contours
                                 [sg.end_points_of_contours.size() - 1];
                         n++) {
                        sg.flags.push_back(read_type<uint8_t>(font_file));
                        std::cout << std::bitset<8>(sg.flags[n]) << "\n";
                    }
                    // using flags get x and y coords
                }
            }
        }
    }
}

uint32_t CmapSubtableFormat4::get_glyph_index(uint16_t unicode_value)
{
    if (unicode_value == 0xFFFF) {
        return 0;
    }
    for (size_t i = 0; i < end_code.size(); i++) {
        if (unicode_value <= end_code[i]) {
            if (unicode_value >= start_code[i]) {
                return id_delta[i] + id_range_offset[i] + unicode_value;
            } else {
                return 0;
            }
        }
    }
    return 0;
}
