#include <plan.h>
std::unique_ptr<bool[]> get_bitmap(const std::byte* page_data, uint16_t num_rows) {
    auto temp = (uint8_t*) page_data + PAGE_SIZE - (num_rows + 7) / 8;
    int size = num_rows/8 + !!(num_rows % 8);
    auto bitmap = std::make_unique<bool[]>(size);
    for (int i = 0; i < size - !!(num_rows%8); ++i) {
        bitmap[i] = static_cast<bool>(temp[i]);
    }
    if (num_rows % 8) {
        uint8_t last_byte = temp[size - 1];
        bitmap[size-1] = last_byte & (-1 << (8 - (num_rows % 8)));
    }
    return bitmap;
}