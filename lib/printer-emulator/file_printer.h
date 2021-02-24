#ifndef FILE_PRINTER_H
#define FILE_PRINTER_H

#include "printer_emulator.h"

class filePrinter : public printer_emu
{
    virtual bool process_buffer(uint8_t linelen, uint8_t aux1, uint8_t aux2) override;
    virtual void post_new_file() override {};
    virtual void pre_close_file() override {};

public:
    filePrinter(paper_t ptype=TRIM) { _paper_type = ptype; };

    virtual const char * modelname() override 
    {
        if(_paper_type == ASCII)
            return "file printer (ASCII)";
        else if (_paper_type == RAW)
            return "file printer (RAW)";
        else
            return "file printer (TRIM)";
    };
};

#endif
