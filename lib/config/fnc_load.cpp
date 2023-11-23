#include "fnConfig.h"
#include "fnSystem.h"

#include "fsFlash.h"

#include <cstring>
#include <sstream>
#include <sys/stat.h>

#include "keys.h"
#include "utils.h"
#include "crypt.h"

#include "debug.h"

/* Load configuration data from FLASH. If no config file exists in FLASH,
   copy it from SD if a copy exists there.
*/
void fnConfig::load()
{
    Debug_printf("fnConfig::load \"%s\"\n", _general.config_file_path.c_str());
        
    struct stat st;
    if (stat(_general.config_file_path.c_str(), &st) < 0)
        {
            _dirty = true; // We have a new (blank) config, so we treat it as needing to be saved
            Debug_println("No config found - starting fresh!");
            return; // No local copy - ABORT
        }
    // Read INI file into buffer (for speed)
    // Then look for sections and handle each
    FILE *fin = fopen(_general.config_file_path.c_str(), FILE_READ_TEXT);
    if (fin == nullptr)
    {
        Debug_printf("Failed to open config file\n");
        return;
    }
    char *inibuffer = (char *)malloc(CONFIG_FILEBUFFSIZE);
    if (inibuffer == nullptr)
    {
        Debug_printf("Failed to allocate %d bytes to read config file\r\n", CONFIG_FILEBUFFSIZE);
        return;
    }
    int i = fread(inibuffer, 1, CONFIG_FILEBUFFSIZE - 1, fin);
    fclose(fin);

    Debug_printf("fnConfig::load read %d bytes from config file\r\n", i);

    if (i < 0)
    {
        Debug_println("Failed to read data from configuration file");
        free(inibuffer);
        return;
    }
    inibuffer[i] = '\0';
    // Put the data in a stringstream
    std::stringstream ss;
    ss << inibuffer;
    free(inibuffer);

    std::string line;
    while (_read_line(ss, line) >= 0)
    {
        int index = 0;
        switch (_find_section_in_line(line, index))
        {
        case SECTION_GENERAL:
            _read_section_general(ss);
            break;
        case SECTION_WIFI:
            _read_section_wifi(ss);
            break;
        case SECTION_WIFI_STORED:
            _read_section_wifi_stored(ss, index);
            break;
        case SECTION_BT:
            _read_section_bt(ss);
            break;
        case SECTION_NETWORK:
            _read_section_network(ss);
            break;
        case SECTION_HOST:
            _read_section_host(ss, index);
            break;
        case SECTION_MOUNT:
            _read_section_mount(ss, index);
            break;
        case SECTION_PRINTER:
            _read_section_printer(ss, index);
            break;
        case SECTION_TAPE: // Oscar put this here to handle server/path to CAS files
            _read_section_tape(ss, index);
            break;
        case SECTION_MODEM:
            _read_section_modem(ss);
            break;
        case SECTION_CASSETTE: //Jeff put this here to handle tape drive configuration (pulldown and play/record)
            _read_section_cassette(ss);
            break;
        case SECTION_CPM:
            _read_section_cpm(ss);
            break;
        case SECTION_PHONEBOOK: //Mauricio put this here to handle the phonebook
            _read_section_phonebook(ss, index);
            break;
        case SECTION_DEVICE_ENABLE: // Thom put this here to handle explicit device enables in adam
            _read_section_device_enable(ss);
            break;
        case SECTION_SERIAL:
            _read_section_serial(ss);
            break;
        case SECTION_NETSIO:
            _read_section_netsio(ss);
            break;
        case SECTION_BOIP:
            _read_section_boip(ss);
            break;
        case SECTION_UNKNOWN:
            break;
        }
    }

    _dirty = false;
}
