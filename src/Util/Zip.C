#include "Zip.h"

#include <archive.h>
#include <archive_entry.h>
#include <iostream>


namespace IQmol {
namespace Util {


int extract_tgz(const char* filename, const char* destPath) {
    struct archive* a = archive_read_new();
    struct archive* ext = archive_write_disk_new();
    struct archive_entry* entry;

    archive_read_support_format_tar(a);
    archive_read_support_filter_gzip(a);  // Enables .gz support

    if (archive_read_open_filename(a, filename, 10240)) {
        std::cerr << "Could not open archive: " << archive_error_string(a) << std::endl;
        return 1;
    }

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* currentFile = archive_entry_pathname(entry);
        std::cout << "Extracting: " << currentFile << std::endl;

        archive_entry_set_pathname(entry, (std::string(destPath) + "/" + currentFile).c_str());

        if (archive_write_header(ext, entry) != ARCHIVE_OK) {
            std::cerr << "Write header failed: " << archive_error_string(ext) << std::endl;
        } else {
            const void* buff;
            size_t size;
            la_int64_t offset;

            while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK) {
                if (archive_write_data_block(ext, buff, size, offset) != ARCHIVE_OK) {
                    std::cerr << "Write data block failed: " << archive_error_string(ext) << std::endl;
                    break;
                }
            }
        }

        archive_write_finish_entry(ext);
    }

    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);

    return 0;
}


} } // end namespace IQmol::Util
