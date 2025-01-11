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
            font_file.seekg(table_directory[i].offset, std::ios::beg);
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
                if (subtable_format == 4 && subtable.platform_id == 0) {
                    CmapSubtableFormat4 cmap_format4{};
                    cmap_format4.length = read_type<uint16_t>(font_file);
                    cmap_format4.language = read_type<uint16_t>(font_file);
                    cmap_format4.seg_count_x2 = read_type<uint16_t>(font_file);
                    std::cout << "seg count: " << cmap_format4.seg_count_x2 / 2
                              << "\n";
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
            font_file.seekg(table_directory[i].offset, std::ios::beg);
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
    std::cout << "loca long: " << head_table.index_to_loc_format << "\n";
    for (auto i = 0; i < table_directory.size(); i++) {
        if (std::string(table_directory[i].tag, 4) == "loca") {
            font_file.seekg(table_directory[i].offset, std::ios::beg);
            for (size_t k = 0; k <= maxp_table.number_glyphs; k++) {
                if (head_table.index_to_loc_format) {
                    auto offset{read_type<uint32_t>(font_file)};
                    loca_table.offsets.push_back(offset);
                } else {
                    loca_table.offsets.push_back(
                        2 *
                        static_cast<uint32_t>(read_type<uint16_t>(font_file))
                    );
                }
            }
        }
    }

    // GLYF
    for (auto i = 0; i < table_directory.size(); i++) {
        if (std::string(table_directory[i].tag, 4) == "glyf") {
            for (auto k = 0; k < loca_table.offsets.size() - 1; k++) {
                font_file.seekg(
                    table_directory[i].offset + loca_table.offsets[k],
                    std::ios::beg
                );
                GlyphCommon gc{};
                gc.number_of_contours = read_type<int16_t>(font_file);
                gc.x_min = read_type<FWord>(font_file);
                gc.y_min = read_type<FWord>(font_file);
                gc.x_max = read_type<FWord>(font_file);
                gc.y_max = read_type<FWord>(font_file);
                if (gc.number_of_contours > 0) {
                    std::cout << "loca_table entry: " << loca_table.offsets[k]
                              << "\n";
                    std::cout << "\t" << gc.number_of_contours << " ["
                              << gc.x_min << ", " << gc.x_max << "] ["
                              << gc.y_min << ", " << gc.y_max << "]";
                    SimpleGlyph sg{};
                    sg.gc = gc;
                    size_t num_points{0};
                    for (size_t n = 0;
                         n < static_cast<size_t>(sg.gc.number_of_contours);
                         n++) {
                        uint16_t end_contour_index =
                            read_type<uint16_t>(font_file);
                        num_points = std::max(
                            static_cast<size_t>(end_contour_index + 1),
                            num_points
                        );
                        sg.end_points_of_contours.push_back(end_contour_index);
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
                    std::cout << "} " << "num_points: " << num_points << " ";

                    sg.instruction_length = read_type<uint16_t>(font_file);
                    for (size_t n = 0; n < sg.instruction_length; n++) {
                        sg.instructions.push_back(read_type<uint8_t>(font_file)
                        );
                    }
                    std::cout << "\n";
                    for (size_t n = 0; n < num_points; n++) {
                        uint8_t flag = read_type<uint8_t>(font_file);
                        sg.flags.push_back(flag);
                        if (sg.flags[n] & 0b1100'0000) {
                            std::cerr << "glyph flags have bits set when they "
                                         "are supposed to be zero\n";
                        }
                        uint8_t constexpr REPEAT{0b0000'1000};
                        if (sg.flags[n] & REPEAT) {
                            uint8_t repeat_count =
                                read_type<uint8_t>(font_file);
                            /*
                            std::cout << "repeat count: " << (int)repeat_count
                                      << "\n";
                                      */
                            for (size_t r = 0; r < repeat_count; r++) {
                                sg.flags.push_back(flag);
                                n++;
                            }
                        }
                    }
                    int16_t x_coord_val{0};
                    for (size_t n = 0; n < sg.flags.size(); n++) {
                        uint8_t constexpr X_SHORT{0b0000'0010};
                        uint8_t constexpr X_SAME{0b0001'0000};
                        if (sg.flags[n] & X_SHORT) {
                            int16_t diff = static_cast<int16_t>(
                                read_type<uint8_t>(font_file)
                            );
                            x_coord_val +=
                                (sg.flags[n] & X_SAME) ? diff : -diff;
                        } else if (!(sg.flags[n] & X_SAME)) {
                            int16_t diff = read_type<int16_t>(font_file);
                            x_coord_val += diff;
                        }
                        sg.x_coords.push_back(x_coord_val);
                    }

                    int16_t y_coord_val{0};
                    for (size_t n = 0; n < sg.flags.size(); n++) {
                        uint8_t constexpr Y_SHORT{0b0000'0100};
                        uint8_t constexpr Y_SAME{0b0010'0000};
                        if (sg.flags[n] & Y_SHORT) {
                            int16_t diff = static_cast<int16_t>(
                                read_type<uint8_t>(font_file)
                            );
                            y_coord_val +=
                                (sg.flags[n] & Y_SAME) ? diff : -diff;
                        } else if (!(sg.flags[n] & Y_SAME)) {
                            int16_t diff = read_type<int16_t>(font_file);
                            y_coord_val += diff;
                        }
                        sg.y_coords.push_back(y_coord_val);
                    }

                    /*
                    std::cout << "x count: " << sg.x_coords.size() << "\n";
                    std::cout << "y count: " << sg.y_coords.size() << "\n\n";
                    for (size_t n = 0; n < sg.x_coords.size(); n++) {
                        std::cout << sg.x_coords[n] << "\t";
                        std::cout << sg.y_coords[n] << "\n";
                    }
                    */
                } else {
                    std::cout << " multi-glyph not implemented\n";
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
