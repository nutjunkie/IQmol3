/*******************************************************************************

  Copyright (C) 2011-2015 Andrew Gilbert

  This file is part of IQmol, a free molecular visualization program. See
  <http://iqmol.org> for more details.

  IQmol is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  IQmol is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along
  with IQmol.  If not, see <http://www.gnu.org/licenses/>.  
   
********************************************************************************/

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
